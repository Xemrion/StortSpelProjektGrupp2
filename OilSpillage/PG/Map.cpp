#include "Map.hpp"
#include "Profiler.hpp"

// TODO: refactor out building gen into separate header|source

Map::Map( Graphics &graphics, MapConfig const &config, Physics *physics ):
	graphics        ( graphics                                                              ),
	physics         ( physics                                                               ),
	config          ( config                                                                ),
	tileMap         ( std::make_unique<TileMap>(config)                                     ),
	districtMarkers ( static_cast<Size>( config.districtCellSide) * config.districtCellSide ),
	roadTiles       ( static_cast<Size>( config.dimensions.x)     * config.dimensions.y     ),
	buildingIDs     ( config.dimensions.x * config.dimensions.y, 0U                         ),
	roadDistanceMap ( config.dimensions.x * config.dimensions.y, 0U                         )
{
	DBG_PROBE(Map::Map);
	// TODO: generate water etc
	generateDistricts();
	generateRoads();
   generateDistanceMap(); // must be done after generateRoads() and before generateBuildings()
	generateBuildings();
}


Map::~Map() noexcept {
	graphics.clearStaticObjects();
	#ifdef _DEBUG
		std::ofstream profilerLogs { String("data/logs/profiler/") + mapConfigToFilename(config, ".txt") }; // TODO: append timestamp?
		assert(profilerLogs.is_open());
		DBG_PROFILE_OUTPUT(profilerLogs); // outputs profiler logs (if in debug mode)
		DBG_PROFILE_RESET();
		profilerLogs.close();
	#endif
}

// TODO: ensure Map.config doesn't stays synchronized during re-rolls.
void  Map::generateRoads() {
	DBG_PROBE(Map::generateRoads);
	RNG      rng{ RD()() };
	I32_Dist generateSeed{};
	rng.seed(config.seed);
	
	MapConfig  roadConfig{ config };
	while (true) { // TODO: add MAX_TRIES?
		RoadGenerator roadGenerator{ *tileMap };
		roadGenerator.generate(roadConfig);
		if (tileMap->getRoadCoverage() < roadConfig.roadMinTotalCoverage) {
			roadConfig.seed = generateSeed(rng);
			tileMap = std::make_unique<TileMap>(roadConfig);
		}
		else {
			startPositionInTileSpace = roadGenerator.getStartPosition();
			break;
		}
	}
	
	if constexpr ( isDebugging ) {
	   std::ofstream roadGenerationLog(String("PG/logs/") + mapConfigToFilename(config, "_road_gen.txt"));
	   if (roadGenerationLog.is_open()) {
		   roadGenerationLog << *tileMap;
		   roadGenerationLog.close();
		}
	}
	
	roadTiles = tileMap->loadAsModels(graphics); // GameObject instantiation
	for ( U16 y = 0;  y < tileMap->height;  ++y ) {
		for ( U16 x = 0;  x < tileMap->width;  ++x ) {
			auto& tile = roadTiles[tileMap->index(x,y)];
			tile.setScale(Vector3{ .0005f * config.tileScaleFactor.x,
			                       .0005f * config.tileScaleFactor.y,
			                       .0005f * config.tileScaleFactor.z }); // TODO: scale models instead
			tile.setNormalMap(graphics.getTexturePointer("brickwallnormal"));
			graphics.addToDraw(&tile , true);
		}
	}
}

void  Map::generateDistricts() {
	DBG_PROBE(Map::generateDistricts);
	RNG  rng{ RD()() };
	rng.seed( config.seed );
	if ( config.isUsingManhattanDistance )
		districtMap = std::make_unique<Voronoi>( rng,
		                                         config.districtCellSide,
		                                         config.dimensions.x / config.districtCellSide,
		                                         config.dimensions.y / config.districtCellSide,
		                                         Voronoi::ManhattanDistanceTag{});
	else districtMap = std::make_unique<Voronoi>( rng,
		                                            config.districtCellSide,
		                                            config.dimensions.x / config.districtCellSide,
		                                            config.dimensions.y / config.districtCellSide,
		                                            Voronoi::EuclideanDistanceTag{});
   if constexpr ( isDebugging ) {
	   // display noise centers:
	   districtMarkers.reserve( districtMap->noise.size() );
	   for ( auto const &cellCentre : districtMap->noise ) {
			districtMarkers.emplace_back();
			auto &marker = districtMarkers.back();
			marker.mesh  = graphics.getMeshPointer("Cube");
			marker.setColor({ 1, 0,  0, 1 });
			marker.setScale({ .2f, 4.0f, .2f });
			marker.setPosition({ tileMap->convertTilePositionToWorldPosition( static_cast<U16>(cellCentre.x),
			                                                                  static_cast<U16>(cellCentre.y)) });
			graphics.addToDraw(&marker, true);
	   }
   }
}

// TODO: make return value optional later instead of asserting
V2u Map::generateRoadPositionInTileSpace(RNG& rng) const noexcept {
   DBG_PROBE(Map::generateRoadPositionInTileSpace);
   static constexpr U16  MAX_TRIES{ 1024 };
   static U16_Dist       generateX(0, config.dimensions.x);
   static U16_Dist       generateY(0, config.dimensions.y);
   U16                   x, y, counter{ 0 };
   do {
      x = generateX( rng );
      y = generateY( rng );
      if ( ++counter > MAX_TRIES )
         assert( false and "No road tile found!" );
   } while ( tileMap->tileAt(x,y) != Tile::road );
   return { x, y };
}

Vector3  Map::generateRoadPositionInWorldSpace(RNG& rng) const noexcept {
	auto  positionInTileSpace{ generateRoadPositionInTileSpace(rng) };
	return tileMap->convertTilePositionToWorldPosition( positionInTileSpace );
}

void  Map::generateBuildings( ) {
	DBG_PROBE( Map::generateBuildings );
	RNG  rng{ RD()() };
	rng.seed( config.seed );
	
	#ifdef _DEBUG
	   U32            total_building_count{ 0 };
	   U32            total_building_tile_count{ 0 };
	   std::ofstream  buildingLogs{ String("PG/logs/building_generation_") + mapConfigToFilename(config, ".txt") };
	   assert(buildingLogs.is_open());
	   buildingLogs << "==================================== BEGINNING BUILDING GENERATION ====================================\n";
	#endif

	U16_Dist  generateOffset(      0,  256 );
	F32_Dist  generateSelection( .0f, 1.0f );

	auto  cellIdToDistrictEnum{ [&generateOffset,&rng]( U16 const ID ) {
	                               return static_cast<District>((ID + generateOffset(rng)) % static_cast<U16>(District::size));
	                            } };

	// generate look-up table (TODO: refactor)
	Vector<District> districtLookupTable{ districtMap->noise.size(), District::park };
	for (U16 i = 0; i < districtLookupTable.size(); ++i)
	{
		districtLookupTable[i] = cellIdToDistrictEnum(i);
	}

	for ( U16 cellId = 0;  cellId < districtMap->noise.size();  ++cellId) {
		District    districtType{ districtLookupTable[cellId] };
		Size        currentArea{ 0 };
		Size const  cellArea{ districtMap->computeCellRealEstateArea(cellId,*tileMap) };
		
		#ifdef _DEBUG
		   U64  currrentDistrictBuildingCount { 0 };
		   buildingLogs << "\n\t" "Generating buildings for district #"
		                << cellId << " (TYPE: \"" << stringify(districtType) << "\")\n";
		#endif
		
		if ( (cellArea != 0) and (districtType != District::park) ) {
			F32  const  targetDistrictCoverage         { District_getBuildingDensity(districtType) };
			auto        computeCurrentDistrictCoverage { [&currentArea, cellArea]() {
			                                                return F32(currentArea) / F32(cellArea);
			                                             } };
			while ( computeCurrentDistrictCoverage() < targetDistrictCoverage ) {
				auto potentialLot = findValidHouseLot( rng, cellId, *districtMap, *tileMap, districtLookupTable );
				if ( potentialLot ) {
					auto &building     = potentialLot.value();
					auto  buildingSize = building.size();
               #ifdef _DEBUG
					   buildingLogs << "\t\t"   "Generating building #" << currrentDistrictBuildingCount++
						   << ". (Map total building count: " << total_building_count++ << ")\n"
						   << "\t\t\t" "Building size: " << buildingSize << " tiles. (Map total tile count: "
						   << (total_building_tile_count += U32(buildingSize)) << ")\n"
						   << "\t\t\t" "The district real estate coverage is now "
						   << (computeCurrentDistrictCoverage() * 100.0f) << "%\n"
						   << "\t\t\t" "The target coverage is: " << targetDistrictCoverage * 100.0f << "%\n";
					#endif
					U16_Dist   generateFloorCount { District_getMinFloorCount(districtType), District_getMaxFloorCount(districtType) };
					F32        randomFloorCount   { static_cast<F32>(generateFloorCount(rng)) };
               BuildingID id                 { generateBuildingID() };
					currentArea += buildingSize;
					for ( auto &tilePosition : potentialLot.value() ) {
						houseTiles.emplace_back();
						auto &houseTile = houseTiles.back();
                  buildingIDs[tileMap->index(tilePosition)] = id;
						tileMap->tileAt(tilePosition) = Tile::building;
						// TODO: assign proper meshes when tilesets have been created
						houseTile.mesh = graphics.getMeshPointer("Cube");
						houseTile.setScale({ .5f * config.tileScaleFactor.x,
						                     .5f * config.tileScaleFactor.y * config.buildingFloorHeightFactor * randomFloorCount,
						                     .5f * config.tileScaleFactor.z });
						houseTile.setPosition({ tileMap->convertTilePositionToWorldPosition(tilePosition) });
						btRigidBody *tmp = physics->addBox( btVector3( houseTile.getPosition().x,
						                                               houseTile.getPosition().y,
						                                               houseTile.getPosition().z ),
						                                    btVector3( houseTile.getScale().x,
						                                               houseTile.getScale().y,
						                                               houseTile.getScale().z ),
						                                    .0f );
						houseTile.setRigidBody( tmp, physics );

                  #ifdef _DEBUG
						   ++total_building_tile_count;
                  #endif
					}
				}
				else break; // TODO?
			}
		}
	}

	// adding all the tiles to draw:
	for ( auto &e : houseTiles )
		graphics.addToDraw(&e , true);

   #ifdef _DEBUG
	   if (buildingLogs.is_open())
		   buildingLogs.close();
   #endif
}

// basic proto placement algorithm
// TODO: refactor out of Game
Opt<Vector<V2u>>  Map::findValidHouseLot( RNG& rng, U16 cellId, Voronoi const& districtMap, TileMap& map, Vector<District> const& districtLookUpTable ) {

   static constexpr auto MAXIMUM_DISTANCE_FROM_ROAD { 3U }; // TODO: refactor into config

   DBG_PROBE( Map::findValidHouseLot );
	using Bounds = Voronoi::Bounds;
	Bounds    cellBounds      { districtMap.computeCellBounds(cellId) };
	U16_Dist  generateOffset  { 0x0, 0xFF };
	U16_Dist  generateX       { cellBounds.min.x, cellBounds.max.x };
	U16_Dist  generateY       { cellBounds.min.y, cellBounds.max.y };
	auto      isValidPosition { [&](V2u const& tilePosition) -> Bool {
                              // TODO: verify roadDistanceMap usage
	                               return ((districtMap.diagram[districtMap.diagramIndex(tilePosition)] != cellId)
	                                    or (map.tileAt(tilePosition) != Tile::ground))
                                      and (roadDistanceMap[map.index(tilePosition)] <= MAXIMUM_DISTANCE_FROM_ROAD);
	                            } };

	District const  districtType       { districtLookUpTable[cellId] }; // TODO: refactor? (and param)
	U16_Dist        generateTargetSize { District_getBuildingMinArea(districtType), District_getBuildingMaxArea(districtType) };
	U16      const  targetSize         { generateTargetSize(rng) };

	// find valid starting coordinate:
	V2u  startPosition{};
	do {
		startPosition.x = generateX( rng );
		startPosition.y = generateY( rng );
	} while (isValidPosition( startPosition) );

	Vector<V2u>  claimedPositions, candidateSources;

	claimedPositions.reserve( targetSize );
	claimedPositions.push_back( startPosition );

	candidateSources.reserve( targetSize );
	candidateSources.push_back( startPosition );

	while (claimedPositions.size() != targetSize and candidateSources.size() != 0) {
		auto const &sourcePosition = candidateSources[ generateOffset(rng) % candidateSources.size() ];
		// find valid neighbours
		auto  candidateDestinations = map.getCardinallyNeighbouringTilePositions(sourcePosition);
		// eliminate invalid ones from our destination candidate list
		candidateDestinations.erase( std::remove_if( candidateDestinations.begin(),
		                                             candidateDestinations.end(),
		                                             isValidPosition ),
		                             candidateDestinations.end());
		// if we found at least one valid destination position, pick one of the candiates
		if (candidateDestinations.size() != 0) {
			// TODO: possibly improve results if weighted towards lower indices?
			auto& match = candidateDestinations[generateOffset(rng) % candidateDestinations.size()];
			claimedPositions.push_back(match);
			candidateSources.push_back(match);
		} // otherwise remove the source position from the source candidates
		else candidateSources.erase( std::remove_if( candidateSources.begin(),
		                                             candidateSources.end(),
		                                             [&sourcePosition](auto const &e) { return e == sourcePosition; }
		                                           ),
		                             candidateSources.end()
		);
	}

	if (claimedPositions.size() == targetSize) return claimedPositions;
	else return {};
}


TileMap const& Map::getTileMap() const noexcept {
	return *tileMap;
}

Voronoi const &Map::getDistrictMap() const noexcept {
   return *districtMap;
}

void  Map::setDistrictColorCoding( bool useColorCoding ) noexcept {
	if ( useColorCoding ) {
		Vector<Vector4> districtColorTable{
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
			rgba = util::blend( rgba, Vector4{ 1.0f }, config.districtColorCodingBlendFactor );
		// colour code tiles depending on cell index:
		for ( U16 y = 0;  y < tileMap->height;  ++y ) {
			for ( U16 x = 0;  x < tileMap->width;  ++x ) {
				assert( tileMap->data.size() == districtMap->diagram.size() and "BUG!" );
				auto       &tile      = roadTiles[ tileMap->index(x, y) ];
				auto        cellIndex = districtMap->diagramIndex(x, y);
				auto        cellId    = districtMap->diagram[ cellIndex ];
				auto const &color     = districtColorTable[ cellId % districtColorTable.size() ];
				tile.setColor( color );
			}
		}
		for ( auto &e : houseTiles ) {
			auto        cellPosition = tileMap->convertWorldPositionToTilePosition( e.getPosition() );
			auto        cellIndex    = districtMap->diagramIndex( cellPosition.x, cellPosition.y );
			auto        cellID       = districtMap->diagram[ cellIndex ];
			auto const &color        = districtColorTable[ cellID % districtColorTable.size() ];
			e.setColor( color );
		}
	}
	else {
		static auto const  defaultColor = Vector4{ .3f, .3f, .3f, 1.0f };
		for ( auto &e : roadTiles )
			e.setColor( defaultColor );
		for ( auto& e : houseTiles )
			e.setColor( defaultColor );
	}
}

V2u  Map::getStartPositionInTileSpace() const noexcept {
	return startPositionInTileSpace;
}

Vector3  Map::getStartPositionInWorldSpace() const noexcept {
	return tileMap->convertTilePositionToWorldPosition( startPositionInTileSpace );
}

void Map::generateDistanceMap() noexcept {
   auto const  maxRadius = util::maxValue( config.dimensions.x, config.dimensions.y );
   auto  getNearestRoad {[=]( auto x, auto y ) {
      auto  radius { 0 };
      do {
         BoundingBox bounds { {x,y}-radius, {x,y}+radius };
         // TODO: refactor into ensureInBounds()?
         if ( bounds.min.x < 0 )
            bounds.min.x = 0;
         if ( bounds.min.y < 0 )
            bounds.min.y = 0;
         if ( bounds.max.x >= config.dimensions.x )
            bounds.max.x = config.dimensions.x;
         if ( bounds.max.y >= config.dimensions.y )
            bounds.max.y = config.dimensions.y;
         // find nearest road:
         for ( auto x = bounds.min.x;  x < bounds.max.x; ++x )
            for ( auto y = bounds.min.y;  x < bounds.max.y; ++y )
               if ( tileMap->tileAt(x,y) == Tile::road )
                  { roadDistanceMap[tileMap->index(x,y)] = radius; break; }
      } while ( ++radius < maxRadius );
      assert( false and "No road found! Bug!" ); // possible solution: check gen params
   }};
}

Map::BuildingID  Map::generateBuildingID() noexcept {
   return nextBuildingID++;
}
