#include "Map.hpp"

Map::Map( Graphics &graphics, MapConfig const &config ):
   graphics        ( graphics ),
   config          ( config   ),
   tilemap         ( std::make_unique<TileMap>(config) ),
   districtMarkers ( static_cast<Size>( config.districtCellSide ) * config.districtCellSide ),
   roadTiles       ( static_cast<Size>( config.dimensions.x     ) * config.dimensions.y     ),
   houseTiles      ( static_cast<Size>( config.dimensions.x     ) * config.dimensions.y / 2 )
{
   // TODO: generate water etc
   generateDistricts();
   generateRoads();
   generateBuildings();
}


Map::~Map() noexcept {
   // TODO: this shouldn't be necessary. make it so GameObject's destructor handles this?
	for ( auto &e : roadTiles )
		graphics.removeFromDraw(&e);
	for ( auto &e : districtMarkers )
		graphics.removeFromDraw(&e);
	for ( auto &e : houseTiles )
		graphics.removeFromDraw(&e);
}


Void  Map::generateRoads() {
   RoadGenerator roadGenerator( *tilemap );
   roadGenerator.generate( config );
   #ifdef _DEBUG
   std::ofstream roadGenerationLog ( Str("PG/logs/")+mapConfigToFilename(config, "_road_gen.txt") );
	   if ( roadGenerationLog.is_open() ) {
		   roadGenerationLog << *tilemap;
		   roadGenerationLog.close();
	   }
   #endif
   roadTiles = tilemap->loadAsModels(graphics); // GameObject instantiation
   for ( U16 y=0;  y < tilemap->height;  ++y ) {
		for ( U16 x=0;  x < tilemap->width;  ++x ) {
			auto &tile = roadTiles[tilemap->index(x,y)];
			graphics.addToDraw(&tile);
			tile.setScale( Vector3{ .0005f * config.tileScaleFactor.x,
			                        .0005f * config.tileScaleFactor.y,
			                        .0005f * config.tileScaleFactor.z }); // TODO: scale models instead
		}
	}
   startPositionInTileSpace = roadGenerator.getStartPosition();
}

Void  Map::generateDistricts() {
   RD   rd;
   RNG  rng( rd() );
   rng.seed( config.seed );
   if ( config.isUsingManhattanDistance )
      districtMap = std::make_unique<Voronoi>( rng,
                                               config.districtCellSide,
                                               config.dimensions.x / config.districtCellSide,
                                               config.dimensions.y / config.districtCellSide,
                                               Voronoi::ManhattanDistanceTag{} );
   else districtMap = std::make_unique<Voronoi>( rng,
                                                 config.districtCellSide,
                                                 config.dimensions.x / config.districtCellSide,
                                                 config.dimensions.y / config.districtCellSide,
                                                 Voronoi::EuclideanDistanceTag{} );
   #ifdef _DEBUG
	   // display noise centers:
	   districtMarkers.reserve( districtMap->noise.size() );
	   for ( auto const &cellCentre : districtMap->noise ) {
		   districtMarkers.emplace_back();
		   auto &marker = districtMarkers.back();
		   marker.mesh  = graphics.getMeshPointer("Cube");
		   marker.setColor({1, 0, 0, 1});
		   marker.setScale({.2f, 4, .2f});
         marker.setPosition({ tilemap->convertTilePositionToWorldPosition( static_cast<U16>(cellCentre.x),
                                                                           static_cast<U16>(cellCentre.y) ) });
		   graphics.addToDraw(&marker);
	   }
   #endif
}

// TODO: make return value optional later instead of asserting
V2u Map::generateRoadPositionInTileSpace( RNG &rng ) const noexcept {
   static constexpr U16 MAX_TRIES = 1024; // TODO: refactor
   static U16_Dist  generateX     ( 0, config.dimensions.x );
   static U16_Dist  generateY     ( 0, config.dimensions.y );
   U16              x, y, counter { 0 };
   do {
      x = generateX( rng );
      y = generateY( rng );
      if ( ++counter > MAX_TRIES )
         assert( false && "No road tile found! ");
   } while ( not tilemap->isRoad( x, y ) );
   return { x, y };
}

Vector3  Map::generateRoadPositionInWorldSpace( RNG &rng ) const noexcept {
   auto positionInTileSpace = generateRoadPositionInTileSpace( rng );
   return tilemap->convertTilePositionToWorldPosition( positionInTileSpace );
}

Void  Map::generateBuildings() {
   RD   rd;
	RNG  rng( rd() );
   rng.seed( config.seed );

#ifdef _DEBUG
	std::ofstream building_logs ( Str("PG/logs/building_generation_")+mapConfigToFilename(config, ".txt") );
	assert( building_logs.is_open() );
	U32  total_building_count      = 0;
	building_logs << "=========================================== BEGINNING BUILDING GENERATION ===========================================\n";
#endif
	U32  total_building_tile_count = 0; // TODO refactor into _DEBUG bloack


	U16_Dist gen_offset    (   0, 256  );
	F32_Dist genSelection ( .0f, 1.0f );
	// TODO: embed into Map to ensure by design that voronoi and map are generated first

	auto cell_id_to_district_enum = [&gen_offset,&rng]( U16 const ID ) {
		return static_cast<District>( (ID+gen_offset(rng)) % static_cast<U16>(District::size) );
	};

	// generate look-up table (TODO: refactor)
	Vec<District> district_tbl { districtMap->noise.size(), District::park };
	for ( auto i = 0;  i < district_tbl.size();  ++i )
		district_tbl[i] = cell_id_to_district_enum(i);

	for ( U16  cell_id = 0;  cell_id < districtMap->noise.size();  ++cell_id ) {
		District    districtType  { district_tbl[cell_id] };
		Size        curr_area      { 0 };
		Size const  cell_area      { districtMap->compute_cell_real_estate_area(cell_id,*tilemap) };

#ifdef _DEBUG
		U64 district_building_count = 0;
		building_logs << "\n\t" "Generating buildings for district #" << cell_id << " (TYPE: \"" << stringify(districtType) << "\")\n";
#endif

		if ( (cell_area != 0) and (districtType != District::park) ) {
			auto        compute_curr_coverage = [&curr_area, cell_area]() { return F32(curr_area)/F32(cell_area); };
			F32  const  target_coverage       { get_building_density(districtType) };
			while ( compute_curr_coverage() < target_coverage ) {
			auto potentialLot = find_valid_house_lot( rng, cell_id, *districtMap, *tilemap, district_tbl );
			if ( potentialLot ) {
				auto &building      = potentialLot.value();
				auto  building_size = building.size();
#ifdef _DEBUG
				building_logs << "\t\t"   "Generating building #" << district_building_count++ << ". (Map total building count: " << total_building_count++ << ")\n"
				              << "\t\t\t" "Building size: "       << building_size << " tiles. (Map total tile count: " << (total_building_tile_count+=U32(building_size)) << ")\n"
				              << "\t\t\t" "The district real estate coverage is now " << (compute_curr_coverage() * 100.0f) << "%\n"
				              << "\t\t\t" "The target coverage is: " << target_coverage * 100.0f << "%\n";
#endif
				U16_Dist  generateFloorCount ( getMinFloorCount(districtType), getMaxFloorCount(districtType) );
            F32       randomFloorCount   { static_cast<F32>( generateFloorCount(rng) ) };
				curr_area += building_size;
				houseTiles.reserve( building_size );
				for ( auto &tilePosition : potentialLot.value() ) {
					houseTiles.emplace_back();
					auto &houseTile = houseTiles.back();
               tilemap->tileAt(tilePosition) = Tile::building;
					houseTile.mesh  = graphics.getMeshPointer("Cube");
					houseTile.setColor( {.75f, .75f, .75f, 1.0f} );
					houseTile.setScale( { 0.5f * config.tileScaleFactor.x,
					                       0.5f * config.tileScaleFactor.y * config.buildingFloorHeightFactor * randomFloorCount,
					                       0.5f * config.tileScaleFactor.z } );
					houseTile.setPosition( { tilemap->convertTilePositionToWorldPosition(tilePosition) } );
					++total_building_tile_count;
				}
			}
			else break; // TODO?
			}
		}
	}

	// adding all the tiles to draw:
	for ( auto &e : houseTiles )
		graphics.addToDraw(&e);

#ifdef _DEBUG
	if ( building_logs.is_open() )
		building_logs.close();
#endif
}

// basic proto placement algorithm
// TODO: refactor out of Game
Opt<Vec<V2u>>  Map::find_valid_house_lot( RNG &rng, U16 cell_id, Voronoi const &district_map, TileMap &map, Vec<District> const &district_tbl ) {
	using Bounds = Voronoi::Bounds;
	Bounds    cell_bounds { district_map.compute_cell_bounds(cell_id) };
	U16_Dist  gen_offset  {               0x0,            0xFF };
	U16_Dist  gen_x       { cell_bounds.min.x, cell_bounds.max.x };
	U16_Dist  gen_y       { cell_bounds.min.y, cell_bounds.max.y };

	auto is_valid_position = [&]( V2u const &pos ) -> bool {
		return  (district_map.diagram[district_map.diagram_index(pos.x,pos.y)] != cell_id)
			or  (map.tileAt(pos.x,pos.y) != Tile::ground);
	};

	District const  district_type    { district_tbl[cell_id] }; // TODO: refactor? (and param)
	U16_Dist        gen_target_size  { get_building_min_area(district_type), get_building_max_area(district_type) };
	U16      const  target_size      { gen_target_size(rng) };

	// find valid starting coordinate:
	V2u  start_pos {};
	do {
		start_pos.x = gen_x(rng);
		start_pos.y = gen_y(rng);
	} while ( is_valid_position(start_pos) );

	Vec<V2u>  claimed_positions, candidate_srcs;

	claimed_positions.reserve( target_size );
	claimed_positions.push_back( start_pos );

	candidate_srcs.reserve( target_size );
	candidate_srcs.push_back( start_pos );

	while ( claimed_positions.size() != target_size  and  candidate_srcs.size() != 0 ) {
		auto const  &src_pos = candidate_srcs[ gen_offset(rng) % candidate_srcs.size() ];
		// find valid neighbours
		auto  candidate_dsts = map.getNeighbouringTilePositions(src_pos);
		// eliminate invalid ones from our destination candidate list
		candidate_dsts.erase( std::remove_if(candidate_dsts.begin(), candidate_dsts.end(), is_valid_position), candidate_dsts.end() );

      // if we found at least one valid destination position, pick one of the candiates
      if ( candidate_dsts.size() != 0 ) {
         // TODO: possibly improve results if weighted towards lower indices?
         auto  &match = candidate_dsts[ gen_offset(rng) % candidate_dsts.size() ];
         claimed_positions.push_back( match );
         candidate_srcs.push_back( match );
      } // otherwise remove the source position from the source candidates
      else candidate_srcs.erase( std::remove_if( candidate_srcs.begin(),
                                                 candidate_srcs.end(),
                                                 [&src_pos] (auto const &e) { return e==src_pos; }
                                 ),
                                 candidate_srcs.end()
      );
   }
   
	if ( claimed_positions.size() == target_size ) return claimed_positions;
	else return {};
}


TileMap const &Map::getTileMap() const noexcept {
   return *tilemap;
}

Void  Map::setDistrictColorCoding( bool useColorCoding ) noexcept {
	if ( useColorCoding ) {
		Vec<Vector4> districtColorTable {
			{ 0.0f, 0.0f, 0.0f, 1.0f },
			{ 0.0f, 0.0f, 0.5f, 1.0f },
			{ 0.0f, 0.0f, 1.0f, 1.0f },

			{ 0.0f, 0.5f, 0.0f, 1.0f },
			{ 0.0f, 0.5f, 0.5f, 1.0f },
			{ 0.0f, 0.5f, 1.0f, 1.0f },

			{ 0.0f, 1.0f, 0.0f, 1.0f },
			{ 0.0f, 1.0f, 0.5f, 1.0f },
			{ 0.0f, 1.0f, 1.0f, 1.0f },

			{ 0.5f, 0.0f, 0.0f, 1.0f },
			{ 0.5f, 0.0f, 0.5f, 1.0f },
			{ 0.5f, 0.0f, 1.0f, 1.0f },

			{ 0.5f, 0.5f, 0.0f, 1.0f },
			{ 0.5f, 0.5f, 0.5f, 1.0f },
			{ 0.5f, 0.5f, 1.0f, 1.0f },

			{ 0.5f, 1.0f, 0.0f, 1.0f },
			{ 0.5f, 1.0f, 0.5f, 1.0f },
			{ 0.5f, 1.0f, 1.0f, 1.0f },

			{ 1.0f, 0.0f, 0.0f, 1.0f },
			{ 1.0f, 0.0f, 0.5f, 1.0f },
			{ 1.0f, 0.0f, 1.0f, 1.0f },

			{ 1.0f, 0.5f, 0.0f, 1.0f },
			{ 1.0f, 0.5f, 0.5f, 1.0f },
			{ 1.0f, 0.5f, 1.0f, 1.0f },

			{ 1.0f, 1.0f, 0.0f, 1.0f },
			{ 1.0f, 1.0f, 0.5f, 1.0f },
			{ 1.0f, 1.0f, 1.0f, 1.0f }
		};
		for ( auto &rgba : districtColorTable )
			rgba = blend( rgba, Vector4{1.0f}, config.districtColorCodingBlendFactor );
		// colour code tiles depending on cell index:
		for ( U16 y=0;  y < tilemap->height;  ++y ) {
			for ( U16 x=0;  x < tilemap->width;  ++x ) {
			   assert( tilemap->data.size() == districtMap->diagram.size() && "BUG!" );
			   auto       &tile     = roadTiles[tilemap->index(x,y)];
			   auto        cell_idx = districtMap->diagram_index(x,y);
			   auto        cell_id  = districtMap->diagram[cell_idx];
			   auto const &color    = districtColorTable[ cell_id % districtColorTable.size()];
			   tile.setColor( color );
			}
		}
		for ( auto &e : houseTiles ) {
			auto        cellPosition = tilemap->convertWorldPositionToTilePosition( e.getPosition() );
			auto        cellIndex    = districtMap->diagram_index(cellPosition.x, cellPosition.y);
			auto        cellID       = districtMap->diagram[cellIndex];
			auto const &color        = districtColorTable[ cellID % districtColorTable.size()];
			e.setColor( color );
		}
	} else {
		static auto const defaultColor = Vector4 { .2f, .2f, .2f, 1.0f };
		for ( auto &e : roadTiles )
			e.setColor( defaultColor );
		for ( auto &e : houseTiles )
			e.setColor( defaultColor );
	}
}

V2u  Map::getStartPositionInTileSpace() const noexcept {
   return startPositionInTileSpace;
}

Vector3  Map::getStartPositionInWorldSpace() const noexcept {
   return tilemap->convertTilePositionToWorldPosition( startPositionInTileSpace );
}
