
#include "utils.hpp"
#include "Map.hpp"
#include "Profiler.hpp"


// NE "outer corner":
//     map     mask    predicate
//     x01     011     001
//     x 0     0 1     0 0
//     xxx     000     000
static U8  constexpr maskOuterC { 0b0000'0111 };
static U8  constexpr predOuterC { 0b0000'0010 };

// N "side":
//     map     mask    predicate
//     x1x     010     010
//     0 0     1 1     0 0
//     xxx     000     000
static U8  constexpr maskSide   { 0b0100'0101 };
static U8  constexpr predSide   { 0b0000'0001 };

// NE "inner corner":
//     map     mask    predicate
// x1x	010	010
// 0 1	1 1	0 1
// x0x	010	000
static U8  constexpr maskInnerC { 0b0101'0101 };
static U8  constexpr predInnerC { 0b0000'0101 };

// N "U":
//     map     mask    predicate
//     x0x     010     000
//     1 1     1 1     1 1
//     x1x     010     010
static U8  constexpr maskU { 0b0101'0101 };
static U8  constexpr predU { 0b0101'0100 };

// "hole":
//     map     mask    predicate
//     x1x     010     010
//     1 1     1 1     1 1
//     x1x     010     010
static U8  constexpr maskHole { 0b0101'0101 };
static U8  constexpr predHole { 0b0101'0101 };

// road bitmaps:

static U8  constexpr maskRoad { 0b0101'0101 };

// "deadend":
//     map     mask    predicate
//     x1x     010     010
//     0 0     1 1     0 0
//     x0x     010     000
static U8  constexpr predDeadend { 0b0000'0001 };


// "bend":
//     map     mask    predicate
//     x1x     010     010
//     0 1     1 1     0 1
//     x0x     010     000
static U8  constexpr predBend { 0b0000'0101 };


// "straight":
//     map     mask    predicate
//     x1x     010     010
//     0 0     1 1     0 0
//     x1x     010     010
static U8  constexpr predStraight { 0b0001'0001 };

// "3-way":
//     map     mask    predicate
//     x1x     010     010
//     1 1     1 1     1 1
//     x0x     010     000
static U8  constexpr pred3Way { 0b0100'0101 };

// "4-way":
//     map     mask    predicate
//     x1x     010     010
//     1 1     1 1     1 1
//     x1x     010     010
static U8  constexpr pred4Way { 0b0101'0101 };

static F32 constexpr baseOffsetY       { -1.50f };
static F32 constexpr markerOffsetY     { -1.49f };
static F32 constexpr zebraOffsetY      { -1.48f };
static F32 constexpr transitionOffsetY { -1.47f };
static F32 constexpr sidewalkOffsetY   { -1.46f };


// 
// 00
// X0  OC
// 
// 
// 10
// X0  SA
// 
// 
// 01
// X0  OC
// 
// 
// 11
// X0  SA
// 
// 
// 00
// X1  SB
// 
// 
// 10
// X1  IC
// 
// 
// 01
// X1  SB
// 
// 
// 11
// X1  MP
// 
// 
// for roof:
//   0 = neighbourFloorNo != myFloorNo
//   1 = neighbourFloorNo == myFloorNo

void Map::generateBorder()
{
	auto constexpr borderThickness = 4;
	auto biome = info.environment.getBiome();
	for ( I32 x = -borderThickness;  x <= I32(tilemap->width)+borderThickness;  ++x ) {
		for ( I32 y = -borderThickness;  y <= I32(tilemap->height)+borderThickness;  ++y ) {
			if ( (x < 0) or (x >= I32(tilemap->width)) or (y < 0) or (y >= I32(tilemap->height)) ) {
				border.meshes.emplace_back();
				auto &ground       = border.meshes.back();
				ground.mesh        = graphics.getMeshPointer("Tiles/Quad_SS");
				ground.setNormalMap(   graphics.getTexturePointer( ( std::string("Tiles/")+stringify(biome)+"_nor").c_str() ) );
				ground.setColor({ .0f, .0f, .0f, .0f });
				ground.setTexture( graphics.getTexturePointer( ( std::string("Tiles/")+stringify(biome)).c_str() ) );
				Vector3  offset     { .0f, baseOffsetY, .0f };
				ground.setPosition( tilemap->convertTilePositionToWorldPosition(x,y) + offset );
				ground.setScale({ config.tileSideScaleFactor, 1.0f, config.tileSideScaleFactor });
			}
		}
	}

	auto constexpr align = -8.0f;
	for ( I32 x = -1;  x <= I32(tilemap->width);  ++x ) {
		for ( I32 y = -1;  y <= I32(tilemap->height);  ++y ) {
			bool isWest  = (x == -1),
			     isEast  = (x == I32(tilemap->width)),
			     isSouth = (y == I32(tilemap->height)),
			     isNorth = (y == -1);
			bool isVertical   = isWest  or isEast,
			     isHorizontal = isNorth or isSouth;
			if ( isVertical or isHorizontal ) {
				border.meshes.emplace_back();
				auto &wallPiece      = border.meshes.back();
				wallPiece.mesh       = graphics.getMeshPointer("Cube");
				wallPiece.setColor({ .75f, .75f, .75f, 0.0f });
				wallPiece.setTexture(   graphics.getTexturePointer("Tiles/concrete"));
				wallPiece.setNormalMap( graphics.getTexturePointer("Tiles/concrete_nor"));
				Vector3  offset       { isVertical? (isWest? -align:align):.0f, -1.5f, isHorizontal? (isNorth? align:-align):.0f};
				wallPiece.setPosition( tilemap->convertTilePositionToWorldPosition(x,y) + offset );
				wallPiece.setScale({ config.tileSideScaleFactor/(isVertical? 8:2), config.tileSideScaleFactor/4, config.tileSideScaleFactor/(isHorizontal? 8:2) });
			}
		}
	}
	for ( auto &e : border.meshes )
		graphics.addToDrawStatic(&e);
	
	// west
	border.bounds[0].setPosition({ tilemap->convertTilePositionToWorldPosition(-1,tilemap->height/2)});
	auto pos0 = border.bounds[0].getPosition();
	auto sca0 = btVector3( config.tileSideScaleFactor/2, 50.0f, (tilemap->height+2)*config.tileSideScaleFactor );
	btRigidBody *tmp0 = physics->addBox( btVector3( pos0.x, pos0.y, pos0.z ),
	                                     btVector3( sca0 ),
	                                     .0f );
	tmp0->setFriction(0);
	border.bounds[0].setRigidBody( tmp0, physics );

	// east
	border.bounds[1].setPosition({ tilemap->convertTilePositionToWorldPosition(tilemap->width,tilemap->height/2)});
	auto pos1 = border.bounds[1].getPosition();
	auto sca1 = btVector3( config.tileSideScaleFactor/2, 50.0f, (tilemap->height+2)*config.tileSideScaleFactor );
	btRigidBody *tmp1 = physics->addBox( btVector3( pos1.x, pos1.y, pos1.z ),
	                                     btVector3( sca1 ),
	                                     .0f );
	tmp1->setFriction(0);
	border.bounds[1].setRigidBody( tmp1, physics );

	// north
	border.bounds[2].setPosition({ tilemap->convertTilePositionToWorldPosition(tilemap->width/2,-1)});
	auto pos2 = border.bounds[2].getPosition();
	auto sca2 = btVector3( (tilemap->width+2)*config.tileSideScaleFactor, 50.0f, config.tileSideScaleFactor/2 );
	btRigidBody *tmp2 = physics->addBox( btVector3( pos2.x, pos2.y, pos2.z ),
	                                     btVector3( sca2 ),
	                                     .0f );
	tmp2->setFriction(0);
	border.bounds[2].setRigidBody( tmp2, physics );

	// south
	border.bounds[3].setPosition({ tilemap->convertTilePositionToWorldPosition(tilemap->width/2,tilemap->height)});
	auto pos3 = border.bounds[3].getPosition();
	auto sca3 = btVector3( (tilemap->width+2)*config.tileSideScaleFactor, 50.0f, config.tileSideScaleFactor/2 );
	btRigidBody *tmp3 = physics->addBox( btVector3( pos3.x, pos3.y, pos3.z ),
	                                     btVector3( sca3 ),
	                                     .0f );
	tmp3->setFriction(0);
	border.bounds[3].setRigidBody( tmp3, physics );
}

void Map::generateZebraCrossings()
{
	F32_Dist  generateSelection { .0f, 1.0f };
	crossingTiles.reserve( 64 );

	auto instantiateCrossing = [&]( Vector3 const &pos, F32 deg=.0f ) {
		crossingTiles.push_back( std::make_unique<GameObject>() );
      auto &obj     =  *crossingTiles.back();
		obj.mesh      =   graphics.getMeshPointer( "Tiles/Quad_SS" );
		obj.setTexture(   graphics.getTexturePointer("Tiles/zebra_crossing") );
		// obj.setNormalMap( graphics.getTexturePointer("Tiles/zebra_crossing_nor") );
		if ( deg > 1.0f )
			obj.setRotation({ .0f, util::degToRad(deg), .0f });
		obj.setPosition( pos + Vector3{.0f, zebraOffsetY, .0f} );
		obj.setColor({ .0f, .0f, .0f, .0f });
		obj.setScale({ tilemap->config.tileSideScaleFactor, 1.0f, tilemap->config.tileSideScaleFactor });
		obj.setSpotShadow( false );
		obj.setSunShadow(  false );
	};

	for ( auto x = 0;  x < (tilemap->width);  x+=2 ) {
		for ( auto y = 0;  y < (tilemap->height);  y+=2 ) {
			if ( (tilemap->tileAt(x,y) == Tile::road)
				and (districtAt(x,y) != &District::park)
				and (districtAt(x,y) != &District::suburban) )
			{
				bool isVertical   = tilemap->neighbourIsRoad( Direction::north, x, y )
									 and tilemap->neighbourIsRoad( Direction::south, x, y );

				bool isHorizontal = tilemap->neighbourIsRoad( Direction::west, x, y )
									 and tilemap->neighbourIsRoad( Direction::east, x, y );

				auto connections  = (tilemap->neighbourIsRoad( Direction::north, x, y )? 1:0)
										+ (tilemap->neighbourIsRoad( Direction::east,  x, y )? 1:0)
										+ (tilemap->neighbourIsRoad( Direction::south, x, y )? 1:0)
										+ (tilemap->neighbourIsRoad( Direction::west,  x, y )? 1:0);
				if ( (connections == 2) and (generateSelection(rng) <= 15.0f) ) {
					if ( isVertical )
						instantiateCrossing( tilemap->convertTilePositionToWorldPosition(x,y), 90.0f );
					else if ( isHorizontal )
						instantiateCrossing( tilemap->convertTilePositionToWorldPosition(x,y), .0f );
				}
			}
		}
	}
	for ( auto &e : crossingTiles ) 
		graphics.addToDrawStatic( e.get() );
}

void Map::generateStreetlights()
{
	auto isValid = [=]( U32 x, U32 y ) {
		auto tile     = tilemap->tileAt(x,y);
		auto district = districtAt(x,y);
		return tile == Tile::road and district != &District::suburban and district != &District::park;
	};

	streetlights.reserve(128);
	for ( auto x = 1;  x < tilemap->width;  ++x ) {
		for ( auto y = 1;  y < tilemap->height;  ++y ) {	
			U8 bitmask = 0x00; // we need to set 2 bits per quadrant to allow cycling (4 quadrants, but 8 bits in a byte)
			bitmask += isValid(x-1, y-1)? (1 +  16) : 0; // NW
			bitmask += isValid(x,   y-1)? (2 +  32) : 0; // NE
			bitmask += isValid(x,   y  )? (4 +  64) : 0; // SE
			bitmask += isValid(x-1, y  )? (8 + 128) : 0; // SW

			Vector3 rotation { .0f, .0f, .0f };
			auto setCount = (bitmask&1) + ((bitmask>>1)&1) + ((bitmask>>2)&1) + ((bitmask>>3)&1);
			if ( setCount == 1 ) {
				for ( auto q = 0;  q < 4;  ++q )
					if ( (util::cycleRight(bitmask,q)&1) == 1 )
						rotation = { .0f, util::degToRad(225.0f+q*90.0f), .0f };
			}
			else if ( setCount == 2 ) {
				for ( auto q = 0;  q < 4;  ++q )
					// 11
					// 00 case:
					if ( (util::cycleRight(bitmask,q)&0b11) == 0b00 )
						rotation = { .0f, util::degToRad(180.0f+q*90.0f), .0f };
					// ignore 01
					//        10 case
			}
			else if ( setCount == 3 ) {
				for ( auto q = 0;  q < 4;  ++q )
					if ( (util::cycleRight(bitmask,q)&1) == 0 )
						rotation = { .0f, util::degToRad(135.0f+q*90.0f), .0f };
			}

			if ( bitmask ) {
				auto worldPosition = tilemap->convertTilePositionToWorldPosition(x,y) - Vector3(tilemap->config.tileSideScaleFactor/2, .0f, tilemap->config.tileSideScaleFactor/-2);
				auto offset = Vector3{ sin(rotation.y), .0f, cos(rotation.y) } * 3.0f;
				worldPosition += offset;
				placeStreetlight( worldPosition, rotation );
			}
		}
	}
}


// TODO: 6. place miscellaneous clutter
//       7. add water tiles
//       8. add building connectors
//       9. add bridges
// -------------------------------------------------
//       i.  integrate L-systems into parks
//       ii. place composite buildings
Vector<HouseTileset> const houseTilesets {
	{ "test", 1.0f, District::all },
};

Vector<MultitileLayout> multitileLayouts {};

auto getCompatibleTilesets( District::Enum d ) noexcept
{
	Vector<HouseTileset const *> matches {};
	for ( auto const &ts: houseTilesets )
		if ( util::isElementOf(ts.validInDistricts, d) )
			matches.push_back( &ts );
	return matches;
}

void loadMultitileLayouts( String const &path )
{
	std::ifstream file;
	file.open(path);
	assert( file.is_open() and "Unable to open layout data file!" );
	U32           cols   { 0 },
	              rows   { 0 };
	Vector<U8>    layout {};
	multitileLayouts.reserve(25);
	for ( std::string line; std::getline(file, line); ) {
		if ( line != "" ) {
			auto prevCols = cols;
			cols = line.length();
			if ( prevCols )
				assert( cols == prevCols );
			for ( char const c: line )
				layout.push_back( std::atoi(&c) );
			++rows;
		}	
		else {
			if ( cols and rows )
				multitileLayouts.push_back({ cols, rows, layout }); // finish current entry
			// reset buffers
			cols = 0;
			rows = 0;
			layout.clear();
		}
	}
	file.close();
}

Opt<const MultitileLayout *> getMultitileLayout( District::Enum d, RNG &rng ) noexcept
{
	for ( U32 attempt = 0;  attempt < 50;  ++attempt ) {
		auto const &e = util::randomElementOf(multitileLayouts,rng);
		if ( *std::max_element(e.floors.begin(), e.floors.end()) > d->maxFloors )
			continue;
      if ( *std::min_element(e.floors.begin(), e.floors.end()) < d->minFloors )
			continue;
		auto area { 0U };
		for ( auto const &f : e.floors )
			++area;
		if ( area < d->minArea )
			continue;
		if ( area > d->maxArea )
			continue;
		return &e;
	}
	return {}; // no valid layout found
}

Map::Map( Graphics &graphics, MapConfig const &config, Physics *physics, LightList &lights                         ):
	graphics        ( graphics                                                                                      ),
	physics         ( physics                                                                                       ),
	lights          ( lights                                                                                        ),
	config          ( config                                                                                        ),
	tilemap         ( nullptr                                                                                       ),
   roadDistanceMap ( config.dimensions.x * config.dimensions.y, 99999.9f                                           ),
	buildingIDs     ( config.dimensions.x * config.dimensions.y                                                     ),
	hospitalTable   ( config.dimensions.x / config.districtCellSide * config.dimensions.y / config.districtCellSide ),
	rng             ( RD()()                                                                                        )
{
	DBG_PROBE(Map::Map);

	if ( config.seed != -1 )  
		rng.seed( config.seed );
	skyscraperGenerator = std::make_unique<Skyscraper>();
	info.environment = {rng};
	info.width       = config.dimensions.x;
	info.length      = config.dimensions.y;

	// TODO: generate water etc
	generateRoads();
	generateRoadDistanceMap();
	generateDistricts();
	generateBuildings();
	generateBorder();
	generateStreetlights();
	generateZebraCrossings();
	instantiateTilesAsModels();

}

Map::~Map() noexcept
{
	graphics.clearStaticObjects();
	for ( auto &e : houses.composites ) {
		physics->deleteRigidBody( e.walls.getRigidBody() );
		physics->deleteRigidBody( e.windows.getRigidBody() );
		physics->deleteRigidBody( e.roof.getRigidBody() );
		skyscraperGenerator->unloadASkyscraper(e.skyscraperMeshIndex);
	}

	for ( auto &e : houses.singles )
		physics->deleteRigidBody( e.object.getRigidBody() );

	for ( auto &e : houses.multis )
		for ( auto &p : e.parts )
			physics->deleteRigidBody( p.getRigidBody() );

	for ( auto &e : border.bounds )
		physics->deleteRigidBody( e.getRigidBody() );

   #ifdef _DEBUG_MAP
		std::ofstream profilerLogs { String("data/logs/profiler/") + mapConfigToFilename(config, ".txt") }; // TODO: append timestamp?
		assert(profilerLogs.is_open());
		DBG_PROFILE_OUTPUT(profilerLogs); // outputs profiler logs (if in debug mode)
		DBG_PROFILE_RESET();
		profilerLogs.close();
	#endif
}

// TODO: ensure Map.config doesn't stays synchronized during re-rolls.
void  Map::generateRoads()
{
	DBG_PROBE( Map::generateRoads );
	I32_Dist   generateSeed {};
	MapConfig  roadConfig { config };
	// generate roads at a lower resolution so we can uspscale the road network to our desired resolution later
	roadConfig.dimensions.x = config.dimensions.x / 2;
	roadConfig.dimensions.y = config.dimensions.y / 2;
	tilemap = std::make_unique<TileMap>( roadConfig );
	for(;;) { // TODO: add MAX_TRIES?
		RoadGenerator roadGenerator{ *tilemap };
		roadGenerator.generate(roadConfig);
		if (tilemap->getRoadCoverage() < roadConfig.roadMinTotalCoverage) {
			roadConfig.seed = generateSeed(rng);
			tilemap = std::make_unique<TileMap>( roadConfig );
		}
		else {
			roadGenerator.upscale();
			startPositionInTileSpace = roadGenerator.getStartPosition();
			break;
		}
	}

	
	if constexpr ( isDebugging ) {
	   std::ofstream roadGenerationLog(String("PG/logs/") + mapConfigToFilename(config, "_road_gen.txt"));
	   if (roadGenerationLog.is_open()) {
		   roadGenerationLog << *tilemap;
		   roadGenerationLog.close();
		}
	}
}


void Map::placeStreetlight( Vector3 const &worldPosition, Vector3 const &rotation ) noexcept
{
	streetlights.emplace_back( std::make_unique<Streetlight>(worldPosition, graphics, lights, rotation) );
}

void  Map::generateDistricts()
{
	DBG_PROBE(Map::generateDistricts);

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

	// generate look-up table (TODO: refactor)
	districtLookupTable = Vector<District::Enum>( districtMap->noise.size() );
	Vector<District::Enum> cityCenterDistricts { &District::residential,
	                                              &District::metropolitan,
	                                              &District::downtown };
	for ( U16 i = 0; i < districtLookupTable.size(); ++i ) {
		auto roadCoverage = districtMap->computeCellRoadCoverage( i, *tilemap );
		if      ( roadCoverage < .05f ) districtLookupTable[i] = &District::park;
		else if ( roadCoverage < .15f ) districtLookupTable[i] = &District::suburban;
		else if ( roadCoverage < .25f ) districtLookupTable[i] = &District::residential;
		else if ( roadCoverage < .30f ) districtLookupTable[i] = &District::downtown;
		else if ( roadCoverage < .40f ) districtLookupTable[i] = util::randomElementOf( cityCenterDistricts, rng );
		else                            districtLookupTable[i] = &District::metropolitan;
	}
}

// TODO: make return value optional later instead of asserting
V2u Map::generateRoadPositionInTileSpace( RNG &rng ) const noexcept {
   //DBG_PROBE(Map::generateRoadPositionInTileSpace);
   DBG_PROBE(Map::generateRoadPositionInTileSpace);
   static constexpr U16  maxTries{ 1024 };
   static U16_Dist       generateX(0, config.dimensions.x-1);
   static U16_Dist       generateY(0, config.dimensions.y-1);
   U16                   x, y, counter{ 0 };
   do {
      x = generateX( rng );
      y = generateY( rng );
      if ( ++counter > maxTries )
         assert( false and "No road tile found!" );
   } while ( tilemap->tileAt(x,y) != Tile::road );
   return { x, y };
}

Vector3  Map::generateRoadPositionInWorldSpace(RNG& rng) const noexcept {
	auto  positionInTileSpace{ generateRoadPositionInTileSpace(rng) };
	return tilemap->convertTilePositionToWorldPosition( positionInTileSpace );
}

V2u Map::generateGroundPositionInTileSpace(RNG& rng) const noexcept
{
	//DBG_PROBE(Map::generateRoadPositionInTileSpace);
	DBG_PROBE(Map::generateRoadPositionInTileSpace);
	static constexpr U16  MAX_TRIES{ 1024 };
	static U16_Dist       generateX(0, config.dimensions.x - 1);
	static U16_Dist       generateY(0, config.dimensions.y - 1);
	U16                   x, y, counter{ 0 };
	do {
		x = generateX(rng);
		y = generateY(rng);
		if (++counter > MAX_TRIES)
			assert(false and "No road tile found!");
	} while (tilemap->tileAt(x, y) != Tile::ground);
	return { x, y };
}

V2u Map::generateNonBuildingPositionInTileSpace(RNG& rng) const noexcept
{
	static constexpr U16  MAX_TRIES{ 1024 };
	static U16_Dist       generateX(0, config.dimensions.x - 1);
	static U16_Dist       generateY(0, config.dimensions.y - 1);
	U16                   x, y, counter{ 0 };
	do {
		x = generateX(rng);
		y = generateY(rng);
		if (++counter > MAX_TRIES)
			assert(false and "No road tile found!");
	} while (tilemap->tileAt(x, y) == Tile::building);
	return { x, y };
}

Vector3 Map::generateNonBuildingPositionInWorldSpace(RNG& rng) const noexcept
{
	auto  positionInTileSpace{ generateNonBuildingPositionInTileSpace(rng) };
	return tilemap->convertTilePositionToWorldPosition(positionInTileSpace);
}
Vector3 Map::generateGroundPositionInWorldSpace(RNG& rng) const noexcept
{
	auto  positionInTileSpace{ generateGroundPositionInTileSpace(rng) };
	return tilemap->convertTilePositionToWorldPosition(positionInTileSpace);
}




// TODO: make district specific like MultitileTilesets
static std::vector singleTileModels {
	"Houses/testHouse",
	"Houses/testHouse2",
	"Houses/testHouse3",
	"Houses/testHouse4",
	"Houses/testHouse5",
	"Houses/testHouse6"
};

static std::vector singleTileModelsDestroyed{
	"Houses/destroyedHouse1",
	"Houses/destroyedHouse2",
	"Houses/destroyedHouse3",
	"Houses/destroyedHouse4",
	"Houses/destroyedHouse5",
	"Houses/destroyedHouse6",
	"Houses/destroyedHouse7"
};

static std::vector singleTileMaterials {
	"Houses/houseMaterial",
	"Houses/houseMaterial2",
	"Houses/houseMaterial3"
};

void  Map::generateBuildings( )
{
	DBG_PROBE(Map::generateBuildings);

	U16 constexpr maxTries { 25U }; // TODO: refactor out
	F32_Dist      randomSizeDist       { .6f, .7f };
	F32 constexpr smallProbability     { .2f      };
	F32_Dist      smallProbabilityDist {};


	U16_Dist  generateOffset(      0,  256 );
	F32_Dist  generateSelection( .0f, 1.0f );

	// for each cell:
	for ( U16 cellId = 0;  cellId < districtMap->noise.size();  ++cellId ) {
		District::Enum district     { districtLookupTable[cellId] };
		Size           currentArea  { 0 };
		Size const     cellArea     { districtMap->computeCellRealEstateArea(cellId,*tilemap) };
		U16_Dist       genNumFloors { district->minFloors, district->maxFloors };

		if ( cellArea != 0 ) { //and (districtType != District::park) ) {
			F32  const  targetDistrictCoverage         { district->buildingDensity };
			auto        computeCurrentDistrictCoverage { [&currentArea, cellArea]() {
			                                                return F32(currentArea) / F32(cellArea);
			                                             } };
         U16 currentTries { 0U };
			auto tilesets = getCompatibleTilesets(district);
			auto multitileCoveragePercentage = district->multitilePercentage * district->buildingDensity;

			// place hospital:
			for ( U32 attempt = 0;  attempt < maxTries;  ++attempt ) {
				auto maybeLot = findFixedLot( cellId, 1, 1, {true} );
				if ( maybeLot ) {
					V2u tilePos { maybeLot.value().nw };
					// ensure the position is valid for a hospital (road adjacent)
					if ( (tilePos.x >                   0) and (tilemap->tileAt(tilePos.x-1, tilePos.y   ) == Tile::road)
					  or (tilePos.y >                   0) and (tilemap->tileAt(tilePos.x,   tilePos.y-1 ) == Tile::road) 
					  or (tilePos.x < tilemap->width  - 1) and (tilemap->tileAt(tilePos.x+1, tilePos.y   ) == Tile::road) 
					  or (tilePos.y < tilemap->height - 1) and (tilemap->tileAt(tilePos.x,   tilePos.y+1 ) == Tile::road) )
					{
						hospitalTable[cellId] = tilePos;
						auto           orientation { getHospitalOrientation(tilePos) };
						F32            yRotation   { .0f };
						F32 constexpr  pi          { 3.14156926535f };
						SingleTileHouse house;
						switch ( orientation ) {
							case Direction::south: { house.object.setRotation({.0f, pi/2,   .0f}); break; } //  90 degs clock-wise Y
							case Direction::west:  { house.object.setRotation({.0f, pi,     .0f}); break; } // 180 degs clock-wise Y
							case Direction::north: { house.object.setRotation({.0f, pi/2*3, .0f}); break; } // 270 degs clock-wise Y
							case Direction::east:  { break; }                                               // no need to rotate
							default: assert( false and "BUG! Unaccounted for direction" );
						}
						house.object.mesh       = graphics.getMeshPointer( "Hospital" );
						house.object.setMaterial( graphics.getMaterial(    "Hospital" ) );
						house.object.setScale({ .048f * config.tileSideScaleFactor,
						                        .048f * config.tileSideScaleFactor,
						                        .048f * config.tileSideScaleFactor });
						house.object.setPosition({ tilemap->convertTilePositionToWorldPosition(tilePos) } );
						#ifndef _DEBUG
							btRigidBody *tmp = physics->addBox( btVector3( house.object.getPosition().x,
							                                               house.object.getPosition().y,
							                                               house.object.getPosition().z ),
							                                    btVector3( 10.5f  * house.object.getScale().x,
							                                               500.0f,
							                                               10.5f  * house.object.getScale().z ),
							                                    .0f );
							tmp->setFriction(0);
							house.object.setRigidBody( tmp, physics );
						#endif
						tilemap->applyLot( maybeLot.value(), Tile::building );
						++currentArea;
						houses.singles.push_back( std::move(house) );
						break;
					}
				}
			}


			// place multi-tile houses:
			while ( (tilesets.size() != 0) and (++currentTries < maxTries)
			       and (computeCurrentDistrictCoverage() < multitileCoveragePercentage ) )
			{
				 //if ( (district == &District::metropolitan) and (generateSelection(rng) < .80f) ) {
				 //	auto house    = instantiateSkyscraper();
				 //	auto maybeLot = findFixedLot( cellId, house.dimensions.x, house.dimensions.y, Vector<Bool>(house.dimensions.x * house.dimensions.y, true));
				 //	if ( maybeLot ) {
				 //		// set affected tiles and discard lot since we won't remove houses dynamically
				 //		currentTries = 0; // reset counter
				 //		currentArea += maybeLot.value().getCoverage();
					//	Vector3 worldPosition = tilemap->convertTilePositionToWorldPosition(maybeLot->nw);
					//	worldPosition.x += house.dimensions.x * config.tileSideScaleFactor * 0.5f;
					//	worldPosition.z += house.dimensions.y * config.tileSideScaleFactor * 0.5f;
					//	house.roof.setPosition(worldPosition);
					//	house.walls.setPosition(worldPosition);
					//	house.windows.setPosition(worldPosition);
				 //		tilemap->applyLot( maybeLot.value(), Tile::building );
				 //		houses.composites.push_back( house ); 
				 //	}
				 //}
				 //else 
				if ( (tilesets.size() != 0) and (generateSelection(rng) < .30f ) ) {
					auto maybeLayout = getMultitileLayout(district,rng);
					if ( maybeLayout ) {
						auto  buildingLayout = maybeLayout.value();
						auto  tileLayout     = Vector<Bool>( Size(buildingLayout->width * buildingLayout->length) );
						for ( auto i = 0;   i < buildingLayout->floors.size();  ++i )
							tileLayout[i] = buildingLayout->floors[i];	
						auto maybeLot = findFixedLot( cellId, buildingLayout->width, buildingLayout->length, std::move(tileLayout) );
						if ( maybeLot ) {
							// set affected tiles and discard lot since we won't remove houses dynamically
							auto const &lot = maybeLot.value();
							currentTries = 0; // reset counter
							currentArea += lot.getCoverage();
							tilemap->applyLot(lot, Tile::building );
							houses.multis.push_back( instantiateMultitileHouse(  lot.nw, MultitileLayout(*maybeLayout.value()), *util::randomElementOf(tilesets,rng) ) );
						}
					}
				}
				else { // random shape:
					auto maybeLot = findRandomLot( cellId );
					if ( maybeLot ) {
						currentTries = 0; // reset counter
						auto const &lot = maybeLot.value();
						currentArea += lot.getCoverage();
						tilemap->applyLot( lot, Tile::building );
						MultitileLayout layout;
						layout.width  = lot.width;
						layout.length = lot.length;
						layout.floors.resize( lot.layout.size() );
						auto numFloors = genNumFloors(rng);
						for ( U32 x = 0;  x < layout.width;  ++x ) {
							for ( U32 y = 0;  y < layout.length;  ++y ) {
								auto index = y * layout.width + x;
								layout.floors[index] = lot.intersects(x+lot.nw.x, y+lot.nw.y)? numFloors : 0;
							}
						}
						houses.multis.push_back( instantiateMultitileHouse( lot.nw, std::move(layout), *util::randomElementOf(tilesets,rng) ) );
					}
				}
			}

			// place single-tile houses:
			while ( (computeCurrentDistrictCoverage() < targetDistrictCoverage) and (++currentTries < maxTries) ) {
				auto maybeLot = findFixedLot( cellId, 1, 1, {true} );
				if ( maybeLot ) {
					currentTries = 0; // reset counter
					SingleTileHouse house;
					house.object.mesh       = graphics.getMeshPointer( util::randomElementOf(singleTileModelsDestroyed,    rng)   );
					house.object.setMaterial( graphics.getMaterial(    util::randomElementOf(singleTileMaterials, rng) ) );
					house.object.setColor({ .0f, .0f, .0f, .0f });
					house.object.setPosition({ tilemap->convertTilePositionToWorldPosition(maybeLot.value().nw) - Vector3(0,2,0) } );
					house.object.setScale({ .0322f * config.tileSideScaleFactor,
						                     .0322f * config.tileSideScaleFactor,
						                     .0322f * config.tileSideScaleFactor });
					#ifndef _DEBUG
						btRigidBody *tmp = physics->addBox( btVector3( house.object.getPosition().x,
						                                               house.object.getPosition().y,
						                                               house.object.getPosition().z ),
						                                    btVector3( 15.5f * house.object.getScale().x,
						                                               500.0f,
						                                               15.5f * house.object.getScale().z ),
						                                    .0f );
						tmp->setFriction(0);
						house.object.setRigidBody( tmp, physics );
					#endif
					tilemap->applyLot( maybeLot.value(), Tile::building );
					houses.singles.push_back( std::move(house) );
					++currentArea;
				}
			}
		}
	}
	/*auto house = instantiateSkyscraper();
	house.roof.setPosition(Vector3(20.0f, 0.0f, -20.0f));
	house.walls.setPosition(Vector3(20.0f, 0.0f, -20.0f));
	house.windows.setPosition(Vector3(20.0f, 0.0f, -20.0f));
	houses.composites.push_back(house);*/
	// adding all the tiles to draw:
	for ( auto &e : houses.composites ) {
		graphics.addToDrawStatic( &e.walls   );
		graphics.addToDrawStatic( &e.windows );
		graphics.addToDrawStatic( &e.roof    );
	}
	for ( auto &e : houses.multis ) {
		for ( auto &p: e.parts )
			graphics.addToDrawStatic( &p ); 
	// DEBUG
		// for ( auto &hb: e.hitboxes ) // REMOVE
		// 	graphics.addToDrawStatic( &hb ); 
	}
	for ( auto &e : houses.singles )
		graphics.addToDrawStatic( &e.object );
}


Opt<Lot>  Map::findFixedLot( U16 districtId, U32 width, U32 length, Vector<Bool> const &&layout ) noexcept {
	DBG_PROBE(Map::findFixedLot);

	assert( districtId < districtMap->diagram.size() and "ID out of bounds!" );

	auto constexpr maxTries { 50U }; // TODO: refactor

	Lot lot;
	lot.width  = width;
	lot.length = length;
	lot.layout = layout;

	Vector<V2u> eliminatedPositions {};

	Bounds districtBounds = districtMap->computeCellBounds(districtId);
	auto genX = U32_Dist { districtBounds.min.x, districtBounds.max.x-width+1  };
	auto genY = U32_Dist { districtBounds.min.y, districtBounds.max.y-length+1 };
	

	for ( U32 attempt = 0;  attempt < maxTries;  ++attempt ) {
		lot.nw = { genX(rng), genY(rng) };
		if ( !util::isElementOf(eliminatedPositions,lot.nw) ) {
			F32 closestRoad = roadDistanceMap[tilemap->index(lot.nw)];
			for ( U32 x = lot.nw.x;  x < lot.nw.x+width;  ++x ) {     // lot space x
				for ( U32 y = lot.nw.y;  y < lot.nw.y+length;  ++y ) { // lot space y
					if ( lot.intersects(x,y) ) {
						if ( (tilemap->tileAt(x,y) != Tile::ground) or (districtMap->idAt(x,y) != districtId) )
							goto end; // early elimination
						else
							closestRoad = util::minValue( closestRoad, roadDistanceMap[tilemap->index(x,y)] );
					}
				}
			}
			if ( closestRoad <= districtLookupTable[districtId]->maxDistFromRoad )
				return lot; // found match!
		}
		end: eliminatedPositions.push_back(lot.nw);		
	}
	return {}; // no valid position found
}



// basic proto placement algorithm

Opt<Lot>  Map::findRandomLot( U16 cellId ) noexcept
 {
   DBG_PROBE(Map::findRandomLot);
   U16 constexpr maxTries { 50U }; // TODO: refactor out

	Bounds          cellBounds      { districtMap->computeCellBounds(cellId) };
	U16_Dist        generateOffset  { 0x0, 0xFFFF };
	U16_Dist        generateX       ( cellBounds.min.x, cellBounds.max.x );
	U16_Dist        generateY       ( cellBounds.min.y, cellBounds.max.y );
	District::Enum  district        { districtLookupTable[cellId] };
	auto isValidPosition { [&](V2u const &tilePosition ) -> Bool {
	                          return (districtMap->idAt(tilePosition) != cellId)
	                              or (tilemap->tileAt(tilePosition) != Tile::ground)
                                 or (roadDistanceMap[tilemap->index(tilePosition)] > district->maxDistFromRoad );
	                       } };

	U16_Dist   generateTargetSize { district->minArea, district->maxArea };
	U16 const  targetSize         { generateTargetSize(rng) };

	// find valid starting coordinate:
   U16  attempt       { 0 };
	V2u  startPosition {};
	do {
		startPosition.x = generateX( rng );
		startPosition.y = generateY( rng );
		if ( ++attempt >= maxTries )
			return {};
	} while ( isValidPosition( startPosition ) );

	Vector<V2u>  claimedPositions, candidateSources;
	Bounds       bounds;

	claimedPositions.reserve(   targetSize    );
	claimedPositions.push_back( startPosition );

	candidateSources.reserve(   targetSize    );
	candidateSources.push_back( startPosition );

	bounds.min.x = startPosition.x;
	bounds.min.y = startPosition.y;
	bounds.max.x = startPosition.x+1;
	bounds.max.y = startPosition.y+1;

	while ( claimedPositions.size() != targetSize and candidateSources.size() != 0 ) {
		auto const &sourcePosition = candidateSources[ generateOffset(rng) % candidateSources.size() ];
		// find valid neighbours
		auto  candidateDestinations = tilemap->getCardinallyNeighbouringTilePositions(sourcePosition);
		// eliminate invalid ones from our destination candidate list
		candidateDestinations.erase( std::remove_if( candidateDestinations.begin(),
		                                             candidateDestinations.end(),
		                                             isValidPosition ),
		                             candidateDestinations.end());
		// if we found at least one valid destination position, pick one of the candiates
		if ( candidateDestinations.size() != 0 ) {
			// TODO: possibly improve results if weighted towards lower indices?
			auto& match = candidateDestinations[generateOffset(rng) % candidateDestinations.size()];
			claimedPositions.push_back(match);
			bounds.min.x = util::minValue( match.x,   bounds.min.x );
			bounds.min.y = util::minValue( match.y,   bounds.min.y );
			bounds.max.x = util::maxValue( match.x+1, bounds.max.x );
			bounds.max.y = util::maxValue( match.y+1, bounds.max.y );
			candidateSources.push_back(match);
		} // otherwise remove the source position from the source candidates
		else candidateSources.erase( std::remove_if( candidateSources.begin(),
		                                             candidateSources.end(),
		                                             [&sourcePosition](auto const &e) { return e == sourcePosition; }
		                                           ),
		                             candidateSources.end()
		);
	}
	// convert to lot data structure
	if ( claimedPositions.size() == targetSize ) {
		Lot lot;
		lot.width  = bounds.max.x - bounds.min.x;
		lot.length = bounds.max.y - bounds.min.y;
		lot.nw     = bounds.min;
		lot.layout = Vector<Bool>( Size(lot.width * lot.length), false );
		for ( auto const &e: claimedPositions )
			lot.layout[ (e.y - lot.nw.y) * lot.width +  (e.x - lot.nw.x) ] = true;
		return lot;
	}
	else return {};
}


TileMap const &Map::getTileMap() const noexcept
{
	return *tilemap;
}

Voronoi const &Map::getDistrictMap() const noexcept
{
   return *districtMap;
}

V2u  Map::getStartPositionInTileSpace() const noexcept
{
	return startPositionInTileSpace;
}

Vector3  Map::getStartPositionInWorldSpace() const noexcept
{
	return tilemap->convertTilePositionToWorldPosition( startPositionInTileSpace );
}

void Map::generateRoadDistanceMap() noexcept
{
	DBG_PROBE(Map::generateRoadDistanceMap);

	U32 const searchRadius = config.distanceMapSearchRadius;

	struct  TileDistanceEntry {
		F32   distance;
		V2u   position;
	};
	auto manhattanDistance = []( V2u const &a, V2u const &b ) {
		return F32( util::abs<I32>( I32(a.x) - b.x )  +  util::abs<I32>( I32(a.y)- b.y ) );
	};

	auto euclideanDistanceSquared = []( V2u const &a, V2u const &b ) {
		return F32(a.x) * a.x * b.x * b.x * a.y * a.y * b.y * b.y;
	};

	auto euclideanDistance = []( V2u const &a, V2u const &b ) {
		return std::sqrt( F32(a.x) * a.x * b.x * b.x * a.y * a.y * b.y * b.y);
	};

	auto &distance_f = manhattanDistance;

	V2u center, nearby;
	for ( center.x = 0;  center.x < tilemap->width;  ++center.x ) {
		for ( center.y = 0;  center.y < tilemap->height;  ++center.y ) {
			if ( tilemap->tileAt(center.x,center.y) == Tile::road ) {
				Bounds bounds;
				bounds.min.x = util::minValue(0U, center.x-searchRadius);
				bounds.min.y = util::minValue(0U, center.y-searchRadius);
				bounds.max.x = util::minValue(center.x+searchRadius, (U32)tilemap->width);
				bounds.max.y = util::minValue(center.y+searchRadius, (U32)tilemap->height);
				for ( nearby.x = bounds.min.x;  nearby.x < bounds.max.x;  ++nearby.x ) {
					for ( nearby.y = bounds.min.y;  nearby.y < bounds.max.y;  ++nearby.y ) {
						if ( tilemap->tileAt(nearby.x, nearby.y) == Tile::ground ) {
							roadDistanceMap[tilemap->index(nearby.x,nearby.y)] = util::minValue(roadDistanceMap[tilemap->index(nearby.x, nearby.y)], distance_f(center, nearby));
						}
					}
				}
			}
		}
	}
		
/*
	V2u centerPos;
	for ( centerPos.x = 0;  centerPos.x < tilemap->width;  ++centerPos.x ) {
		for ( centerPos.y = 0;  centerPos.y < tilemap->height;  ++centerPos.y ) {
			Bounds const bounds {
				V2u( util::maxValue<I32>(I32(centerPos.x) - searchRadius, 0),
					  util::maxValue<I32>(I32(centerPos.y) - searchRadius, 0) ),
				V2u( util::minValue(centerPos.x + searchRadius, static_cast<U32>(config.dimensions.x-1)),
					  util::minValue(centerPos.y + searchRadius, static_cast<U32>(config.dimensions.y-1)) )
			};

			Size const boundsWidth      = bounds.max.x - bounds.min.x;
			Size const boundsHeight     = bounds.max.y - bounds.min.y;
			Size const numTilesInBounds = boundsWidth * boundsHeight;

			V2u boundsLocalPos;
			for ( boundsLocalPos.x = 0;  boundsLocalPos.x < boundsWidth;  ++boundsLocalPos.x ) {
				for ( boundsLocalPos.y = 0;  boundsLocalPos.y < boundsHeight;  ++boundsLocalPos.y ) {
					V2u const candidatePos { boundsLocalPos + bounds.min };
					candidates[boundsLocalPos.y * boundsWidth + boundsLocalPos.x] = {
						distance_f( candidatePos, centerPos ),
						candidatePos
					};

					// sort by distance to current center tile
					std::sort( candidates.begin(),
								  candidates.begin() + numTilesInBounds,
								  [] ( TileDistanceEntry const &lhs, TileDistanceEntry const &rhs ) {
									  return lhs.distance < rhs.distance;
								  }
					);

					// check if we found a road
					Opt<F32> foundRoadDistance {};
					auto     end               { candidates.begin() + numTilesInBounds };
					for ( auto it = candidates.begin();  not foundRoadDistance and it < end;  ++it )
						if ( Tile::road == tilemap->tileAt(it->position) )
							foundRoadDistance = it->distance;

					// assign found distance or some default distance
					roadDistanceMap[tilemap->index(centerPos)] = foundRoadDistance.value_or( distance_f({0,0}, {U32(searchRadius)+1, U32(searchRadius)+1}) );
				}
			}
		}
	}*/
}

Vector<F32> const& Map::getRoadDistanceMap() const noexcept
{
	return roadDistanceMap;
}

Map::BuildingID  Map::generateBuildingID() noexcept
{
   return nextBuildingID++;
}

Vector<Opt<V2u>> const &Map::getHospitalTable() const noexcept
{
	return hospitalTable;
}


Opt<V2u> Map::getNearestFoundHospitalTilePos( Vector3 const &sourceWorldPos, UIPlaying const &ui ) const noexcept
{
   Opt<V2u> bestMatch;
   F32      bestDistanceSqr;
   for ( auto const &maybeHospitalTilePos : hospitalTable ) {
      if ( maybeHospitalTilePos ) {
         auto hospitalTilePos  = maybeHospitalTilePos.value();
         auto hospitalWorldPos    = tilemap->convertTilePositionToWorldPosition(hospitalTilePos);
         if ( ui.hasExploredOnMinimap(hospitalWorldPos) ) {
            auto hospitalDistanceSqr = Vector3::DistanceSquared( sourceWorldPos, hospitalWorldPos );
            if ( !bestMatch ) { // first match case
               bestMatch       = maybeHospitalTilePos;
               bestDistanceSqr = hospitalDistanceSqr;
            }
            else if ( hospitalDistanceSqr < bestDistanceSqr ) {
               bestDistanceSqr = hospitalDistanceSqr;
               bestMatch       = maybeHospitalTilePos;
            }
         }
      }
   }
   return bestMatch;
}

Direction Map::getHospitalOrientation( V2u const xy ) const noexcept
{
   if      ( (xy.y < tilemap->height-1) and (tilemap->tileAt(xy.x, xy.y+1) == Tile::road) )
      return Direction::south;
   else if ( (xy.x < tilemap->width-1)  and (tilemap->tileAt(xy.x+1, xy.y) == Tile::road) )
      return Direction::east;
   else if ( (xy.x > 0) and (tilemap->tileAt(xy.x-1, xy.y) == Tile::road) )
      return Direction::west;
   else if ( (xy.y > 0) and (tilemap->tileAt(xy.x, xy.y-1) == Tile::road) )
      return Direction::north;
   else assert( false and "Invalid hospital position!" );
}

Vector3 Map::getHospitalFrontPosition( V2u const hospitalTilePos ) const noexcept
{
   // TODO: bryt ut tileScaleFactor x/z till en float tileSideScaleFactor i MapConfig
   F32 offsetDistance { config.tileSideScaleFactor / 1.6f }; // TODO: tune!
   auto orientation = getHospitalOrientation( hospitalTilePos );
   auto result = tilemap->convertTilePositionToWorldPosition( hospitalTilePos );
   switch ( orientation ) { 
      case Direction::south: result += Vector3(  .0f, .0f,  -offsetDistance ); break;
      case Direction::west:  result += Vector3( -offsetDistance, .0f, .0f   ); break;
      case Direction::east:  result += Vector3(  offsetDistance, .0f, .0f   ); break;
      case Direction::north: result += Vector3(  .0f, .0f, offsetDistance   ); break;
   }
   return result;
}

District::Enum  Map::districtAt( U32 x, U32 y ) const noexcept {
	auto tileIdx = districtMap->diagramIndex(x,y);
	auto cellIdx = districtMap->diagram[ tileIdx ];
	return districtLookupTable[ cellIdx ];
}








// Wraps a tile's type and encodes the possible existence of neighbouring road tiles in the 8 eight directions into bits.
struct TileInfo {
   NeighbourMask   roadmap, housemap, concreteMap;
	Vector3         origin;
	Tile            tileType;
	District::Enum  districtType;
};

inline Vector<TileInfo> extractTileInfo( Map const &map ) noexcept {
	auto const &tilemap = map.getTileMap();
	Vector<TileInfo>  results  { tilemap.data.size() };
	for ( auto y=0;  y < tilemap.height;  ++y ) {
		for ( auto x=0;  x < tilemap.width;  ++x ) {
			auto &entry        = results[ tilemap.index(x,y) ];
			entry.origin       = tilemap.convertTilePositionToWorldPosition(x,y);
			entry.tileType     = tilemap.tileAt(x,y);
			entry.districtType = map.districtAt(x,y);
			if ( (y > 0) and (tilemap.tileAt(x,y-1)==Tile::road) )
				entry.roadmap.n  = true;
			if ( (x < tilemap.width-1) and (y > 0) and (tilemap.tileAt(x+1,y-1)==Tile::road) )
				entry.roadmap.ne = true;
			if ( (x < tilemap.width-1) and (tilemap.tileAt(x+1,y)==Tile::road) )
				entry.roadmap.e  = true;
			if ( (x < tilemap.width-1) and (y < tilemap.height-1) and (tilemap.tileAt(x+1,y+1)==Tile::road) )
				entry.roadmap.se = true;
			if ( (y < tilemap.height-1) and (tilemap.tileAt(x,y+1)==Tile::road) )
				entry.roadmap.s  = true;
			if ( (x > 0) and (y < tilemap.height-1) and (tilemap.tileAt(x-1,y+1)==Tile::road) )
				entry.roadmap.sw = true;
			if ( (x > 0) and (tilemap.tileAt(x-1,y)==Tile::road) )
				entry.roadmap.w  = true;
			if ( (x > 0) and (y > 0) and (tilemap.tileAt(x-1,y-1)==Tile::road) )
				entry.roadmap.nw = true;

			if ( (y > 0) and (tilemap.tileAt(x,y-1)==Tile::building) )
				entry.housemap.n  = true;
			if ( (x < tilemap.width-1) and (y > 0) and (tilemap.tileAt(x+1,y-1)==Tile::building) )
				entry.housemap.ne = true;
			if ( (x < tilemap.width-1) and (tilemap.tileAt(x+1,y)==Tile::building) )
				entry.housemap.e  = true;
			if ( (x < tilemap.width-1) and (y < tilemap.height-1) and (tilemap.tileAt(x+1,y+1)==Tile::building) )
				entry.housemap.se = true;
			if ( (y < tilemap.height-1) and (tilemap.tileAt(x,y+1)==Tile::building) )
				entry.housemap.s  = true;
			if ( (x > 0) and (y < tilemap.height-1) and (tilemap.tileAt(x-1,y+1)==Tile::building) )
				entry.housemap.sw = true;
			if ( (x > 0) and (tilemap.tileAt(x-1,y)==Tile::building) )
				entry.housemap.w  = true;
			if ( (x > 0) and (y > 0) and (tilemap.tileAt(x-1,y-1)==Tile::building) )
				entry.housemap.nw = true;



			/* COMPUTE ENTRY CONCRETE MAP */ {

				// d <=> 0b{ NW W SW S ' SE E NE N }

				// used to account for road sidewalk edge cases:
				auto districtRoadsHaveSidewalk = []( District::Enum d ) constexpr {
					return d == &District::metropolitan
					    or d == &District::downtown
					    or d == &District::residential;
				};

				if (
					( entry.districtType == &District::metropolitan and entry.tileType     != Tile::road )
				or ( entry.districtType == &District::residential   or entry.districtType == &District::downtown)
                 and entry.tileType == Tile::building )
				{
					entry.concreteMap.bitmap = 0x00; // no sidewalk borders on these tiles! (they're full concrete)
				}
				else for ( U8 d=0b0000'0001;  d;  d<<=1 ) {
					// compute position of neighbour tile in direction d:
					U32 nX = x,
						 nY = y;
					if      ( d & 0b1000'0011 ) --nY;
					else if ( d & 0b0011'1000 ) ++nY;
					if      ( d & 0b1110'0000 ) --nX;
					else if ( d & 0b1000'1110 ) ++nX;
					// check bounds:
					if ( (nX < tilemap.width) and (nY < tilemap.height) ) {
						// in-bounds:
						auto nDistrict = map.districtAt(nX,nY);
						auto nTile     = tilemap.tileAt(nX,nY);

						if ( (entry.districtType == &District::residential or entry.districtType == &District::downtown)
                           and (entry.tileType==Tile::ground and nTile!=Tile::ground) )
						{
							entry.concreteMap.bitmap |= d; // outer sidewalk! (TEMP)
						}
						else {
							if ( (nDistrict == &District::metropolitan)
							 or ((nDistrict == &District::residential or nDistrict == &District::downtown) and nTile==Tile::building) )
							{
								entry.concreteMap.bitmap |= d; // concrete!
							}
							else entry.concreteMap.bitmap &= ~d;

							if ( d & 0b0101'0101 ) { // if d is a cardinal direction:
								if ( entry.tileType == Tile::road ) {
									if ( nTile == Tile::road )
										entry.concreteMap.bitmap &= ~d; // clear concrete map's side bit in direction d
									else if ( districtRoadsHaveSidewalk(entry.districtType) )
										entry.concreteMap.bitmap |= d; // sidewalk!

									// account for district (no road sidewalk -> road sidewalk) transition edge case:
									if ( !districtRoadsHaveSidewalk(entry.districtType) ) {
										if ( districtRoadsHaveSidewalk(nDistrict) ) {
											entry.concreteMap.bitmap |= util::cycleLeft(  d,1 ); // set direction  left corner bit
											entry.concreteMap.bitmap |= util::cycleRight( d,1 ); // set direction right corner bit
										}
									}
								}
							}

							if ( (  entry.districtType == &District::metropolitan
								  or entry.districtType == &District::residential
								  or entry.districtType == &District::downtown )
							and entry.tileType==Tile::road and (nTile!=Tile::road or ( d & 0b1010'1010 )) /* corner! */ )
							{
									entry.concreteMap.bitmap |= d; // inner sidewalk!
							}
						}
					}
					else { // out of bounds:
						if ( entry.tileType == Tile::road and districtRoadsHaveSidewalk(entry.districtType) )
							entry.concreteMap.bitmap |= d;    // out of bounds, current tile is sidewalked road, set bit!
						else entry.concreteMap.bitmap &= ~d; // out-of-bounds, not sidewalked road, no concrete in direction d
					}
				}
			}
		}
	}
	return results;
}


/*
void Map::instantiateHousesAsModels() noexcept {
	// estimate number of  house parts
	Size  counter = houses.composites.size()*3 + houses.singles.size();
	Size width, height;
	Size x, y;

	auto index = [&width,height]( Size x, Size y ) {
		return y * width + x;
	};

	for ( auto &multi : houses.multis ) {
		width   = multi.bounds.max.x - multi.bounds.min.x;
		height  = multi.bounds.max.y - multi.bounds.min.y;
		for ( x = 0;  x < width;  ++x ) {
			for ( y = 0;  y < height;  ++y ) {
				U8   tilemask   = 0x00;
				auto floorCount = multi.layout[index(x,y)];
				if ( multi.layout[index(x,y)] != 0 ) {
					// compute mask:
					if ( (y-1 < height)
					and (multi.layout[index(x,y-1)]   < floorCount ) )
						tilemask += 1 << 0; // N
					if ( (y-1 < height) and (x+1 < width)
					and (multi.layout[index(x+1,y-1)] < floorCount ) )
						tilemask += 1 << 1; // NE
					if ( (x+1 < width)
					and (multi.layout[index(x+1,y)]   < floorCount ) )
						tilemask += 1 << 2; // E
					if ( (y+1 < height) and (x+1 < width)
					and (multi.layout[index(x+1,y-1)] < floorCount ) )
						tilemask += 1 << 3; // SE
					if ( (y+1 < height)
					and (multi.layout[index(x,y+1)]   < floorCount ) )
						tilemask += 1 << 4; // S
					if ( (y+1 < height) and (x-1 < width)
					and (multi.layout[index(x+1,y+1)] < floorCount ) )
						tilemask += 1 << 5; // SW
					if ( (x-1 < width)
					and (multi.layout[index(x-1,y)]   < floorCount ) )
						tilemask += 1 << 6; // W
					if ( (y-1 < height) and (x-1 < width)
					and (multi.layout[index(x-1,y-1)] < floorCount ) )
						tilemask += 1 << 7; // NW
					// process mask:
					for ( auto q=0; q<4; ++q ) { // quadrants
						U8 quadmask = (tilemask >> (2 * q)) & 0b111;
						if ( (quadmask & 0b101) == 0 )  // outer corner
							counter += floorCount + 2;
						else if ( quadmask == 0b101 ) // inner corner
							counter += floorCount + 2;
						else if ( quadmask == 0b100 ) // side A
							counter += floorCount + 2;
						else if ( quadmask == 0b001 ) // side B
							counter += floorCount + 2;
						else if ( quadmask = 0b111 )  // midpiece
							++counter; // roof only
					}
				}
			}
		}
	}

	Vector<GameObject> parts;
	parts.reserve( counter );

	auto instantiatePart = [&]( std::string name, Vector3 const &pos, F32 deg=.0f, F32 yOffset=.0f, Vector3 scaleFactor=Vector3(1.0f, 1.0f, 1.0f) ) {
		parts.emplace_back();		
		auto &model      = parts.back();
		model.mesh       = graphics.getMeshPointer(  name.c_str() );
		model.setMaterial( graphics.getMaterial(     name.c_str() ) );
		if ( deg > 1.0f )
			model.setRotation({ .0f, util::degToRad(deg), .0f });
		model.setPosition( pos + Vector3{.0f, yOffset, .0f} );
		model.setColor({ .0f, .0f, .0f, .0f });
		model.setScale( scaleFactor );
	};

	// adding multitile house parts:
	for ( auto &multi : houses.multis ) {
		width   = multi.bounds.max.x - multi.bounds.min.x;
		height  = multi.bounds.max.y - multi.bounds.min.y;
		for ( x = 0;  x < width;  ++x ) {
			for ( y = 0;  y < height;  ++y ) {
				U8   tilemask   = 0x00;
				auto floorCount = multi.layout[index(x,y)];
				if ( multi.layout[index(x,y)] != 0 ) {
					// compute mask:
					if ( (y-1 < height)
					and (multi.layout[index(x,y-1)]   < floorCount ) )
						tilemask += 1 << 0; // N
					if ( (y-1 < height) and (x+1 < width)
					and (multi.layout[index(x+1,y-1)] < floorCount ) )
						tilemask += 1 << 1; // NE
					if ( (x+1 < width)
					and (multi.layout[index(x+1,y)]   < floorCount ) )
						tilemask += 1 << 2; // E
					if ( (y+1 < height) and (x+1 < width)
					and (multi.layout[index(x+1,y-1)] < floorCount ) )
						tilemask += 1 << 3; // SE
					if ( (y+1 < height)
					and (multi.layout[index(x,y+1)]   < floorCount ) )
						tilemask += 1 << 4; // S
					if ( (y+1 < height) and (x-1 < width)
					and (multi.layout[index(x+1,y+1)] < floorCount ) )
						tilemask += 1 << 5; // SW
					if ( (x-1 < width)
					and (multi.layout[index(x-1,y)]   < floorCount ) )
						tilemask += 1 << 6; // W
					if ( (y-1 < height) and (x-1 < width)
					and (multi.layout[index(x-1,y-1)] < floorCount ) )
						tilemask += 1 << 7; // NW
					// process mask:
					for ( auto q=0; q<4; ++q ) { // quadrants
						U8 quadmask = (tilemask >> (2 * q)) & 0b111;
						if ( (quadmask & 0b101) == 0x00 ) {
							instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/f_oc", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, .0f );
							for ( auto currFloor = 0; currFloor < floorCount; ++currFloor )
								instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/w_oc", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, currFloor*multi.tileset.floorHeight );
							instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/r_oc", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, floorCount*multi.tileset.floorHeight );
						}
						else if ( quadmask == 0b101 ) {
							 instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/f_ic", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, .0f );
							for ( auto currFloor = 0; currFloor < floorCount; ++currFloor )
								instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/w_ic", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, currFloor*multi.tileset.floorHeight );
							instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/r_ic", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, floorCount*multi.tileset.floorHeight );
						}
						else if ( quadmask == 0b100 ) {
							instantiatePart( "Houses/tilesets/"+multi.tileset.name+"/f_sa", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, .0f );
							for ( auto currFloor = 0; currFloor < floorCount; ++currFloor )
								instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/w_sa", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, currFloor*multi.tileset.floorHeight );
							instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/r_sa", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, floorCount*multi.tileset.floorHeight );
						}
						else if ( quadmask == 0b001 ) {
							instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/f_sb", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, .0f );
							for ( auto currFloor = 0; currFloor < floorCount; ++currFloor )
								instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/w_sb", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, currFloor*multi.tileset.floorHeight );
							instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/r_sb", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, floorCount*multi.tileset.floorHeight );
						}
						else if ( quadmask = 0b111 ) {
							instantiatePart( "/Houses/tilesets/"+multi.tileset.name+"/r_m", tilemap->convertTilePositionToWorldPosition(x,y), .0f, floorCount*multi.tileset.floorHeight );
						}
					}
				}
			}
		}
	}

	// adding single tile houses:
	// TODO

	// adding composite houses:
	// TODO

}*/

MultiTileHouse  Map::instantiateMultitileHouse( V2u const &nw, MultitileLayout &&layout, HouseTileset const &tileset ) const noexcept
{
	auto  index = [&layout]( U32 x, U32 y) { return y * layout.width + x; };
	// generate masks and compute the final number of parts:
	auto  masks       = Vector<NeighbourMask>( Size(layout.width) * layout.length );
	U32   numParts    = { 0 };
	U32   numHitboxes = { 0 };
	for ( U32 x = 0;  x < layout.width;  ++x ) {
		for ( U32 y = 0;  y < layout.length;  ++y ) {
			U32  idx    = index(x,y);
			auto &mask  = masks[idx];
			mask.bitmap = 0x00;
			auto floorCount = layout.floors[idx];
			if ( layout.floors[idx] != 0 ) {
				// compute mask:
				mask.n  = ((y-1 < layout.length) and                          (layout.floors[index( x,   y-1 )] >= floorCount ));
				mask.ne = ((y-1 < layout.length) and (x+1 < layout.width) and (layout.floors[index( x+1, y-1 )] >= floorCount ));
				mask.e  = (                          (x+1 < layout.width) and (layout.floors[index( x+1, y   )] >= floorCount ));
				mask.se = ((y+1 < layout.length) and (x+1 < layout.width) and (layout.floors[index( x+1, y+1 )] >= floorCount ));
				mask.s  = ((y+1 < layout.length) and                          (layout.floors[index( x,   y+1 )] >= floorCount ));
				mask.sw = ((y+1 < layout.length) and (x-1 < layout.width) and (layout.floors[index( x-1, y+1 )] >= floorCount ));
				mask.w  = (                          (x-1 < layout.width) and (layout.floors[index( x-1, y   )] >= floorCount ));
				mask.nw = ((y-1 < layout.length) and (x-1 < layout.width) and (layout.floors[index( x-1, y-1 )] >= floorCount ));
				// process mask for parts count:
				for ( auto q=0; q<4; ++q ) { // quadrants
					U8 quadmask = util::cycleRight( mask.bitmap, 2*q) & 0b00000'111;
					if      ( (quadmask & 0b00000'101) == 0b00000'000 ) { // 010 or 000 => outer corner
						numParts += floorCount + 2;
						++numHitboxes;
					}
					else if ( (quadmask & 0b00000'111) == 0b00000'101 ) { // 101 => inner corner
						numParts += floorCount + 2;
						numHitboxes += 2;
					}
					else if ( (quadmask & 0b00000'101) == 0b00000'100 ) { // 100 or 110 => side A
						numParts += floorCount + 2;
						++numHitboxes;
					}
					else if ( (quadmask & 0b00000'101) == 0b00000'001 ) { // 001 or 010 => side B
						numParts += floorCount + 2;
						++numHitboxes;
					}
					else if ( (quadmask & 0b00000'111) == 0b00000'111 ) { // 111 => midpiece
						++numParts; // roof only
					}
				}
			}
		}
	}

	MultiTileHouse  house;
	house.parts = {};
	house.parts.reserve(numParts);
	house.hitboxes = {};
	house.hitboxes.reserve(numHitboxes+20); // hack
	house.nw     = nw;
	house.layout = std::move(layout);

	auto instantiateTilePart = [&]( std::string const &partName, Vector3 const &pos, F32 deg, U8 floor, F32 floorHeightFactor=1.0f ) {
		house.parts.emplace_back();
		auto &model      = house.parts.back();
		std::string name = std::string("Houses/tilesets/")+tileset.name+"/"+partName;
		model.mesh       = graphics.getMeshPointer( name.c_str() );
		model.setMaterial( graphics.getMaterial(    name.c_str() ) );
		if ( deg > 1.0f )
			model.setRotation({ .0f, util::degToRad(deg), .0f });

		Vector4 color  { .0f, .0f, .0f, .0f };
		Vector3 offset {};
//	debug colour coding
//		if      ( std::round(deg) ==   0 ) {
//			//offset = { .5,   0, .5f };
//			color  = { 1.0f, .0f, .0f, 1.0f }; // red
//		}
//		else if ( std::round(deg) ==  90 ) {
//			//offset = { .5, -99, .5f };
//			color  = { .0f, 1.0f, .0f, 1.0f }; // green
//		}
//		else if ( std::round(deg) == 180 ) {
//			//offset = { .5, -99, .5f };
//			color  = { .0f, .0f, 1.0f, 1.0f }; // blue
//		}
//		else if ( std::round(deg) == 270 ) {
//			//offset = { .5, -99, .5f };
//			color  = { 1.0f, 1.0f, .0f, 1.0f }; // yellow
//		}
		model.setPosition( pos + Vector3{.0f, -1.5f+(config.buildingHeightScaleFactor*floorHeightFactor*0.5f*floor), .0f} + offset );
		model.setColor( color );
		model.setScale({ config.tileSideScaleFactor*0.005f,
		                 config.buildingHeightScaleFactor * 0.005f * floorHeightFactor,
		                 config.tileSideScaleFactor*0.005f });
	};

	constexpr auto quadrant_southeast = 0, // 00
	               quadrant_southwest = 1, // 01
	               quadrant_northwest = 2, // 10
	               quadrant_northeast = 3; // 11	               

	F32 const fullSide = tilemap->config.tileSideScaleFactor / 2;
	F32 const halfSide = fullSide / 2;
	F32 const fracSide = halfSide * 0.25;
	for ( U32 x = 0;  x < house.layout.width;  ++x ) {
		for ( U32 y = 0;  y < house.layout.length;  ++y ) {
			auto idx          = index(x,y);
			auto mask         = masks[idx];
			auto floorCount   = house.layout.floors[idx];
			auto basePosition = tilemap->convertTilePositionToWorldPosition(nw.x+x, nw.y+y);
			// process mask:
			U8 quadmask = mask.bitmap;
			if (mask.bitmap != 0x00) for ( auto q=0;  q<4;  ++q ) { // quadrants
				quadmask = util::cycleRight(quadmask, 2);
				if      ( (quadmask & 0b00000'101) == 0x00000'000 ) { // 000 or 010 => outer corner
					instantiateTilePart( "f_oc", basePosition, 90.0f*q, .0f );
					#ifndef _DEBUG // add rigid body to quadrant
						house.hitboxes.emplace_back();
						auto &hitbox = house.hitboxes.back();
						auto     sca = Vector3 { fracSide*2.5f, 10.0f, fracSide*2.5f };
						auto     pos = basePosition;
						if ( (q == quadrant_northeast) or (q == quadrant_northwest) )
							pos.z += (2.5*fracSide);
						else // south
							pos.z -= (2.5*fracSide);
						if ( (q == quadrant_northwest) or (q == quadrant_southwest) )
							pos.x -= (2.5*fracSide);
						else // south
							pos.x += (2.5*fracSide);

					// DEBUG
						// hitbox.setColor({1.0f, .0f, .0f, 1.0f}); // red
						// hitbox.setPosition(pos);
						// hitbox.mesh = graphics.getMeshPointer("Cube");
						// hitbox.setScale(sca);

						btRigidBody *tmp = physics->addBox( btVector3( pos.x, pos.y, pos.z ),
						                                    btVector3( sca.x, sca.y, sca.z ),
						                                   .0f );
						tmp->setFriction(0);
						hitbox.setRigidBody( tmp, physics );
					#endif
					for ( auto currFloor = 0; currFloor < floorCount; ++currFloor )
						instantiateTilePart( "w_oc", basePosition, 90.0f*q, currFloor,  tileset.floorHeight );
					instantiateTilePart(    "r_oc", basePosition, 90.0f*q, floorCount-1, tileset.floorHeight );
				}
				else if ( (quadmask & 0b00000'111) == 0b00000'101 ) { // 101 => inner corner
						instantiateTilePart( "f_ic", basePosition, 90.0f*q, .0f );
					for ( auto currFloor = 0; currFloor < floorCount; ++currFloor ) 
						instantiateTilePart( "w_ic", basePosition, 90.0f*q, currFloor,  tileset.floorHeight );

					#ifndef _DEBUG // add rigid body to quadrant
						bool  isVertical = (q==quadrant_southeast) or (q==quadrant_northwest);
						auto        scaB = Vector3 { isVertical? halfSide:fracSide, 10.0f, isVertical? fracSide:halfSide };
						auto        posA = basePosition;
						auto        scaA = Vector3 { isVertical? fracSide:halfSide, 10.0f, isVertical? halfSide:fracSide };
						auto        posB = basePosition;
						switch (q) {
							case quadrant_southeast: posA.z -= halfSide; posA.x += halfSide;   posB.x += halfSide; posB.z -= halfSide;  break;
							case quadrant_southwest: posA.x -= halfSide; posA.z -= halfSide;   posB.z -= halfSide; posB.x -= halfSide;  break;
							case quadrant_northwest: posA.z += halfSide; posA.x -= halfSide;   posB.x -= halfSide; posB.z += halfSide;  break;
							case quadrant_northeast: posA.x += halfSide; posA.z += halfSide;   posB.z += halfSide; posB.x += halfSide;  break;
						}

						house.hitboxes.emplace_back();
						auto    &hitboxA = house.hitboxes.back();

					// DEBUG
						// hitboxA.setColor({1.0f, 1.0f, .0f, 1.0f}); // yellow
						// hitboxA.setPosition(posA);
						// hitboxA.mesh = graphics.getMeshPointer("Cube");
						// hitboxA.setScale(scaA);

						btRigidBody *tmpA = physics->addBox( btVector3( posA.x, posA.y, posA.z ),
						                                     btVector3( scaA.x, scaA.y, scaA.z ),
						                                    .0f );
						tmpA->setFriction(0);
						hitboxA.setRigidBody( tmpA, physics );


						house.hitboxes.emplace_back();
						auto  &hitboxB = house.hitboxes.back();

					// DEBUG
						//	hitboxB.setColor({1.0f, 0.0f, 1.0f, 1.0f}); // magenta
						//	hitboxB.setPosition(posB);
						//	hitboxB.mesh = graphics.getMeshPointer("Cube");
						//	hitboxB.setScale(scaB);

						btRigidBody *tmpB = physics->addBox( btVector3( posB.x, posB.y, posB.z ),
						                                     btVector3( scaB.x, scaB.y, scaB.z ),
						                                    .0f );
						tmpB->setFriction(0);
						hitboxB.setRigidBody( tmpB, physics );
					#endif

					instantiateTilePart( "r_ic", basePosition, 90.0f*q, floorCount-1, tileset.floorHeight );
				}
				else if ( (quadmask & 0b00000'101) == 0b00000'100 ) { // 100 or 110 => side A
					instantiateTilePart( "f_sa", basePosition, 90.0f*q, .0f );
					#ifndef _DEBUG // add rigid body to quadrant
						house.hitboxes.emplace_back();
						auto    &hitbox = house.hitboxes.back();
						bool isVertical = (q==quadrant_southeast) or (q==quadrant_northwest);
						auto        sca = Vector3 { isVertical? fracSide:halfSide, 10.0f, isVertical? halfSide:fracSide };
						auto        pos = basePosition;
						switch (q) {
							case quadrant_southeast: pos.x += halfSide;  pos.z -= halfSide;  break;
							case quadrant_southwest: pos.z -= halfSide;  pos.x -= halfSide;  break;
							case quadrant_northwest: pos.x -= halfSide;  pos.z += halfSide;  break;
							case quadrant_northeast: pos.z += halfSide;  pos.x += halfSide;  break;
						}

					// DEBUG
						//	hitbox.setColor({.0f, 1.0f, .0f, 1.0f}); // green
						//	hitbox.setPosition(pos);
						//	hitbox.mesh = graphics.getMeshPointer("Cube");
						//	hitbox.setScale(sca);

						btRigidBody *tmp = physics->addBox( btVector3( pos.x, pos.y, pos.z ),
						                                    btVector3( sca.x, sca.y, sca.z ),
						                                   .0f );
						tmp->setFriction(0);
						hitbox.setRigidBody( tmp, physics );
					#endif
					for ( auto currFloor = 0; currFloor < floorCount; ++currFloor )
						instantiateTilePart( "w_sa", basePosition, 90.0f*q, currFloor,  tileset.floorHeight );
					instantiateTilePart(    "r_sa", basePosition, 90.0f*q, floorCount-1, tileset.floorHeight );
				}
				else if ( (quadmask & 0b00000'101) == 0b00000'001 ) { // 001 or 011 => side B
					instantiateTilePart(    "f_sb", basePosition, 90.0f*q, .0f );
					#ifndef _DEBUG // add rigid body to quadrant
						house.hitboxes.emplace_back();
						auto    &hitbox = house.hitboxes.back();
						bool isVertical = (q==quadrant_southeast) or (q==quadrant_northwest);
						auto        sca = Vector3 { isVertical? halfSide:fracSide, 10.0f, isVertical? fracSide:halfSide };
						auto        pos = basePosition;
						switch (q) {
							case quadrant_southeast: pos.z -= halfSide;  pos.x += halfSide;  break;
							case quadrant_southwest: pos.x -= halfSide;  pos.z -= halfSide;  break;
							case quadrant_northwest: pos.z += halfSide;  pos.x -= halfSide;  break;
							case quadrant_northeast: pos.x += halfSide;  pos.z += halfSide;  break;
						}

					// DEBUG
						// hitbox.setColor({.0f, 0.0f, 1.0f, 1.0f}); // blues
						// hitbox.setPosition(pos);
						// hitbox.mesh = graphics.getMeshPointer("Cube");
						// hitbox.setScale(sca);

						btRigidBody *tmp = physics->addBox( btVector3( pos.x, pos.y, pos.z ),
						                                    btVector3( sca.x, sca.y, sca.z ),
						                                   .0f );
						tmp->setFriction(0);
						hitbox.setRigidBody( tmp, physics );
					#endif
					for ( auto currFloor = 0; currFloor < floorCount; ++currFloor )
						instantiateTilePart( "w_sb", basePosition, 90.0f*q, currFloor,  tileset.floorHeight );
					instantiateTilePart(    "r_sb", basePosition, 90.0f*q, floorCount-1, tileset.floorHeight );
				}
				else if ( (quadmask & 0b00000'111) == 0b00000'111 ) { // 111 => midpiece
					instantiateTilePart(     "r_m", basePosition, 90.0f*q, floorCount-1, tileset.floorHeight );
				}
			}
		}
	}
	//for ( auto &part : house.parts )
	//	part.addRotation({ .0f, 3.1415926535f, .0f  });

	return std::move( house );
}

CompositeHouse Map::instantiateSkyscraper()
{
	static U32 nrOfSkyscrapers = 0;
	String skyscraperName = "skyscraper" + std::to_string(nrOfSkyscrapers++);
	skyscraperGenerator->generateASkyscraper(skyscraperName);

	CompositeHouse temp;
	temp.skyscraperMeshIndex = skyscraperName;

	skyscraperGenerator->setRoofMesh(temp.skyscraperMeshIndex, &temp.roof);
	skyscraperGenerator->setWallMesh(temp.skyscraperMeshIndex, &temp.walls);
	skyscraperGenerator->setWindowMesh(temp.skyscraperMeshIndex, &temp.windows);
	temp.roof.setScale(Vector3(2.0f, 1.0f, 2.0f));
	temp.walls.setScale(Vector3(2.0f, 1.0f, 2.0f));
	temp.windows.setScale(Vector3(2.0f, 1.0f, 2.0f));
	temp.roof.setTexture(graphics.getTexturePointer("skyscraper-wall02"));
	temp.walls.setTexture(graphics.getTexturePointer("skyscraper-wall01"));
	temp.windows.setTexture(graphics.getTexturePointer("skyscraper-window01"));

	Vector3 tempVec = temp.walls.getAABB().maxPos - temp.walls.getAABB().minPos;
	tempVec.x *= 2;
	tempVec.z *= 2;
	temp.dimensions.x = (tempVec.x / config.tileSideScaleFactor);
	temp.dimensions.y = (tempVec.z / config.tileSideScaleFactor);

	return temp;
}


void Map::instantiateTilesAsModels() noexcept
{
	auto biome = info.environment.getBiome();

	groundTiles.reserve( tilemap->data.size() * 3 );// TODO: use worst case scenario: 6 quads per tile?
	auto instantiatePart = [&]( std::string_view name, Vector3 const &pos, F32 deg=.0f, F32 yOffset=baseOffsetY, Bool hasNormal=true, Bool noShadowcasting=false ) {
		groundTiles.push_back( std::make_unique<GameObject>() );
      auto &model     =  *groundTiles.back();
		model.mesh      =   graphics.getMeshPointer( "Tiles/Quad_SS" );
		model.setTexture(   graphics.getTexturePointer(name.data()) );
		if ( hasNormal )
			model.setNormalMap( graphics.getTexturePointer( (std::string(name)+"_nor").c_str() ) );
		if ( deg > 1.0f )
			model.setRotation({ .0f, util::degToRad(deg), .0f });
		model.setPosition( pos + Vector3{.0f, yOffset, .0f} );
		model.setColor({ .0f, .0f, .0f, .0f });
		model.setScale({ tilemap->config.tileSideScaleFactor, 1.0f, tilemap->config.tileSideScaleFactor });
		model.setSpotShadow( noShadowcasting );
		model.setSunShadow(  noShadowcasting );
	};

	F32_Dist genSelection {};

	for ( auto const &e : extractTileInfo(*this) ) {
		// place eventual outer sidewalk borders:
		if ( (e.concreteMap.bitmap & maskHole) == predHole )
				instantiatePart( "Tiles/sidewalk_hole", e.origin, sidewalkOffsetY );
		else for ( auto d=0;  d<8;  d+=2 ) {
			if ( (e.concreteMap.bitmap & util::cycleLeft( maskOuterC, d )) == util::cycleLeft(predOuterC,d) )
				instantiatePart( "Tiles/sidewalk_corner_outer_ne", e.origin, 45.0f*d, sidewalkOffsetY );
			if ( (e.concreteMap.bitmap & util::cycleLeft( maskSide,   d )) == util::cycleLeft(predSide,d) )
				instantiatePart( "Tiles/sidewalk_side_n", e.origin, 45.0f*d, sidewalkOffsetY );
			if ( (e.concreteMap.bitmap & util::cycleLeft( maskInnerC, d )) == util::cycleLeft(predInnerC,d) )
				instantiatePart( "Tiles/sidewalk_corner_inner_ne", e.origin, 45.0f*d, sidewalkOffsetY );
			if ( (e.concreteMap.bitmap & util::cycleLeft( maskU,      d )) == util::cycleLeft(predU,d) )
				instantiatePart( "Tiles/sidewalk_u_n", e.origin, 45.0f*d, sidewalkOffsetY );
		}

		if ( e.tileType == Tile::building ) {
			if ( e.districtType == &District::metropolitan
           or e.districtType == &District::downtown
           or e.districtType == &District::residential )
				instantiatePart( "Tiles/concrete", e.origin );
			else if ( e.districtType == &District::suburban 
                or e.districtType == &District::park )
			{ // no full-size concrete under buildings in these districts
				instantiatePart( "Tiles/"+stringify(biome), e.origin );
			}
		}
			
		else if ( e.tileType == Tile::ground ) {
		// intrinsic: center c == 0
			if ( e.districtType == &District::metropolitan )
			{ // no grass tiles in metropolitan districts
				instantiatePart( "Tiles/concrete", e.origin );
			}
			else if ( e.districtType == &District::downtown
                or e.districtType == &District::residential
                or e.districtType == &District::suburban
					 or e.districtType == &District::park )
			{
				instantiatePart( "Tiles/"+stringify(biome), e.origin );
			}
		}
		else if ( e.tileType == Tile::road ) {
		// intrinsic: center c == 1
			if ( e.districtType == &District::metropolitan
           or e.districtType == &District::downtown
           or e.districtType == &District::residential )
			{	// ^ these districts have 4-lane roads
				// place eventual inner sidewalk borders:

				// place eventual road lines:
				auto maskedMap = e.roadmap.bitmap & maskRoad;
				if ( maskedMap == pred4Way ) {
					instantiatePart( "Tiles/road_marker_4way", e.origin, .0f, markerOffsetY, false );
				}
				if ( maskedMap == predStraight )
					instantiatePart( "Tiles/road_marker_straight_n", e.origin, .0f, markerOffsetY, false );
				else if ( maskedMap == util::cycleLeft( predStraight, 2 ) )
					instantiatePart( "Tiles/road_marker_straight_n", e.origin, 90.0f, markerOffsetY, false );
				else for ( auto d=0; d<8; d+=2 ) {
					if ( maskedMap == util::cycleLeft( pred3Way, d ) ) {
						instantiatePart( "Tiles/road_marker_3way_n", e.origin, 45.0f*d, markerOffsetY, false );
						break;
					}
					if ( maskedMap == util::cycleLeft( predBend, d ) ) {
						instantiatePart( "Tiles/road_marker_turn_ne", e.origin, 45.0f*d, markerOffsetY, false );
						break;
					}
					if ( maskedMap == util::cycleLeft( predDeadend, d ) ) {
						instantiatePart( "Tiles/road_marker_deadend_n", e.origin, 180.0f+45.0f*d, markerOffsetY, false );	
						break;
					}
				}
				instantiatePart( "Tiles/asphalt", e.origin );
			}
			else if ( e.districtType == &District::suburban or e.districtType == &District::park ) {
				// only 2-lane roads in suburban districts
				auto maskedMap = e.roadmap.bitmap & maskRoad;
				if ( maskedMap == pred4Way )
					instantiatePart( "Tiles/road_2file_4way", e.origin, .0f, markerOffsetY );
				if ( maskedMap == predStraight )
					instantiatePart( "Tiles/road_2file_straight_n", e.origin, .0f, markerOffsetY );
				else if ( maskedMap == util::cycleLeft( predStraight, 2 ) )
					instantiatePart( "Tiles/road_2file_straight_n", e.origin, 90.0f, markerOffsetY );
				else for ( auto d=0; d<8; d+=2 ) {
					if ( maskedMap == util::cycleLeft( pred3Way, d ) ) {
						instantiatePart( "Tiles/road_2file_3way_n", e.origin, 45.0f*d, markerOffsetY );
						break;
					}
					if ( maskedMap == util::cycleLeft( predBend, d ) ) {
						instantiatePart( "Tiles/road_2file_turn_ne", e.origin, 45.0f*d, markerOffsetY );
						break;
					}
					if ( maskedMap == util::cycleLeft( predDeadend, d ) ) {
						instantiatePart( "Tiles/road_2file_deadend_n", e.origin, 180.f+45.0f*d, markerOffsetY );
						break;
					}
				}
				instantiatePart( "Tiles/"+stringify(biome), e.origin );
			}
		}
		else if ( e.districtType == &District::park ) {
			instantiatePart( "Tiles/"+stringify(biome), e.origin );
			// TODO: park roads using L-systems
		}
	}

	// generate transitions:
	for ( auto y=0;  y < tilemap->height;  ++y ) {
		for ( auto x=0;  x < tilemap->width;  ++x ) {
			if ( tilemap->tileAt(x,y) == Tile::road ) {
				auto thisDistrict = districtAt(x,y);
				auto thisPos      = tilemap->convertTilePositionToWorldPosition(x,y);
				if ( thisDistrict == &District::suburban or thisDistrict == &District::park ) {
					if ( y-1 < tilemap->height and tilemap->tileAt(x,y-1) == Tile::road and districtAt(x,y-1) != &District::park and districtAt(x,y-1) != &District::suburban )
						instantiatePart( "Tiles/road_trans_2file2metro", (thisPos+tilemap->convertTilePositionToWorldPosition(x,y-1))/2,    .0f, transitionOffsetY );
					if ( x+1 < tilemap->width  and tilemap->tileAt(x+1,y) == Tile::road and districtAt(x+1,y) != &District::park and districtAt(x+1,y) != &District::suburban )
						instantiatePart( "Tiles/road_trans_2file2metro", (thisPos+tilemap->convertTilePositionToWorldPosition(x+1,y))/2,  90.0f, transitionOffsetY );
					if ( y+1 < tilemap->height and tilemap->tileAt(x,y+1) == Tile::road and districtAt(x,y+1) != &District::park and districtAt(x,y+1) != &District::suburban )
						instantiatePart( "Tiles/road_trans_2file2metro", (thisPos+tilemap->convertTilePositionToWorldPosition(x,y+1))/2, 180.0f, transitionOffsetY );
					if ( x-1 < tilemap->width  and tilemap->tileAt(x-1,y) == Tile::road and districtAt(x-1,y) != &District::park and districtAt(x-1,y) != &District::suburban )
						instantiatePart( "Tiles/road_trans_2file2metro", (thisPos+tilemap->convertTilePositionToWorldPosition(x-1,y))/2, 270.0f, transitionOffsetY );
				}
			}
		}
	}
	for ( auto &e : groundTiles ) 
		graphics.addToDrawStatic( e.get() );
}

HouseGenData const& Map::getHouseData() const noexcept
{
	return houses;
}

Map::Info const& Map::getInfo() const noexcept
{
	return info;
}
