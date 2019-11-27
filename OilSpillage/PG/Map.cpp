#include "utils.hpp"
#include "Map.hpp"
#include "Profiler.hpp"


// TODO: 1. make road coverage affect district type
//       2. scale up road maps
//       3. improve base road tileset
//       3. build BxW house placer
//       4. add secondary road tileset + transitions
//       5. add proper house tilesets
//       6. place miscellaneous clutter
//       7. add water tiles
//       8. add building connectors
//       9. add bridges
// -------------------------------------------------
//       i.  integrate L-systems into parks
//       ii. place composite buildings


// for transition screen
struct MapInfo {
   U8       act, stage;
	Biome    biome;
	Size     size;
	//Weather  weather;
	//Time     time;
	String   name;
	// ...
};


Map::Map( Graphics &graphics, MapConfig const &config, Physics *physics ):
	graphics        ( graphics                                                                                      ),
	physics         ( physics                                                                                       ),
	config          ( config                                                                                        ),
	tilemap         ( std::make_unique<TileMap>( config )                                                           ),
   roadDistanceMap ( config.dimensions.x * config.dimensions.y                                                     ),
	buildingIDs     ( config.dimensions.x * config.dimensions.y                                                     ),
	hospitalTable	 ( config.dimensions.x / config.districtCellSide * config.dimensions.y / config.districtCellSide )
{
	DBG_PROBE(Map::Map);
	biome = Biome::desert;
	// TODO: generate water etc
	generateRoads();
	generateRoadDistanceMap();
	generateDistricts();
	generateBuildings();

	groundTiles = instantiateTilesAsModels();
	for ( auto &e : groundTiles ) 
		graphics.addToDrawStatic( e.get() );
}


Map::~Map() noexcept
{
	graphics.clearStaticObjects();
	for ( auto &e : houseTiles )
		physics->DeleteRigidBody( e.getRigidBody() );
	//for ( auto &e : houseTiles )
	//	graphics.removeFromDraw( &e );
	//for ( auto &e : groundTiles )
	//	graphics.removeFromDraw( e.get() );
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
	DBG_PROBE(Map::generateRoads);
	RNG      rng{ RD()() };
	I32_Dist generateSeed{};
	rng.seed(config.seed);
	
	MapConfig  roadConfig { config };
	for(;;) { // TODO: add MAX_TRIES?
		RoadGenerator roadGenerator{ *tilemap };
		roadGenerator.generate(roadConfig);
		if (tilemap->getRoadCoverage() < roadConfig.roadMinTotalCoverage) {
			roadConfig.seed = generateSeed(rng);
			tilemap = std::make_unique<TileMap>( roadConfig );
		}
		else {
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

void  Map::generateDistricts()
{
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

	// generate look-up table (TODO: refactor)
	districtLookupTable = Vector<District::Type const *>( districtMap->noise.size() );
	Vector<District::Type const *> cityCenterDistricts { &District::residential,
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
		std::cout << (int)districtLookupTable[i];
	}
}

// TODO: make return value optional later instead of asserting
V2u Map::generateRoadPositionInTileSpace( RNG &rng ) const noexcept {
   //DBG_PROBE(Map::generateRoadPositionInTileSpace);
   DBG_PROBE(Map::generateRoadPositionInTileSpace);
   static constexpr U16  MAX_TRIES{ 1024 };
   static U16_Dist       generateX(0, config.dimensions.x-1);
   static U16_Dist       generateY(0, config.dimensions.y-1);
   U16                   x, y, counter{ 0 };
   do {
      x = generateX( rng );
      y = generateY( rng );
      if ( ++counter > MAX_TRIES )
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

Vector3 Map::generateGroundPositionInWorldSpace(RNG& rng) const noexcept
{
	auto  positionInTileSpace{ generateGroundPositionInTileSpace(rng) };
	return tilemap->convertTilePositionToWorldPosition(positionInTileSpace);
}

void  Map::generateBuildings( )
{
	DBG_PROBE(Map::generateBuildings);

	U16 const maxTries { 25U }; // TODO: refactor out

	RNG  rng{ RD()() };
	rng.seed( config.seed );
	F32_Dist      randomSizeDist       { .6f, .7f };
	F32 constexpr smallProbability     { .2f      };
	F32_Dist      smallProbabilityDist {};

	//Array of possible buildings
	std::string buildingArr[6];
	buildingArr[0] = "Houses/testHouse";
	buildingArr[1] = "Houses/testHouse2";
	buildingArr[2] = "Houses/testHouse3";
	buildingArr[3] = "Houses/testHouse4";
	buildingArr[4] = "Houses/testHouse5";
	buildingArr[5] = "Houses/testHouse6";
	
	std::string textureArr[3];
	textureArr[0] = "Houses/houseMaterial";
	textureArr[1] = "Houses/houseMaterial2";
	textureArr[2] = "Houses/houseMaterial3";

	#ifdef _DEBUG_MAP
	   U32            total_building_count{ 0 };
	   U32            total_building_tile_count{ 0 };
	   std::ofstream  buildingLogs{ String("PG/logs/building_generation_") + mapConfigToFilename(config, ".txt") };
	   assert(buildingLogs.is_open());
	   buildingLogs << "==================================== BEGINNING BUILDING GENERATION ====================================\n";
	#endif

	U16_Dist  generateOffset(      0,  256 );
	F32_Dist  generateSelection( .0f, 1.0f );

	// for each cell:
	for ( U16 cellId = 0;  cellId < districtMap->noise.size();  ++cellId) {
		District::Type const *district{ districtLookupTable[cellId] };
		Size        currentArea{ 0 };
		Size const  cellArea{ districtMap->computeCellRealEstateArea(cellId,*tilemap) };
		
		#ifdef _DEBUG_MAP
		   U64  currrentDistrictBuildingCount { 0 };
		   buildingLogs << "\n\t" "Generating buildings for district #"
		                << cellId << " (TYPE: \"" << district->name << "\")\n";
		#endif
		
		if ( (cellArea != 0) ) { //and (districtType != District::park) ) {
			F32  const  targetDistrictCoverage         { district->buildingDensity };
			auto        computeCurrentDistrictCoverage { [&currentArea, cellArea]() {
			                                                return F32(currentArea) / F32(cellArea);
			                                             } };
         U16 currentTries { 0U };
			while ( computeCurrentDistrictCoverage() < targetDistrictCoverage and ++currentTries < maxTries ) {
				auto potentialLot = findValidHouseLot(rng, cellId, *districtMap, *tilemap );
				if ( potentialLot ) {
               currentTries = 0; // reset try counter
					auto &building     = potentialLot.value();
					auto  buildingSize = building.size();
               #ifdef _DEBUG_MAP
					   buildingLogs << "\t\t"   "Generating building #" << currrentDistrictBuildingCount++
						   << ". (Map total building count: " << total_building_count++ << ")\n"
						   << "\t\t\t" "Building size: " << buildingSize << " tiles. (Map total tile count: "
						   << (total_building_tile_count += U32(buildingSize)) << ")\n"
						   << "\t\t\t" "The district real estate coverage is now "
						   << (computeCurrentDistrictCoverage() * 100.0f) << "%\n"
						   << "\t\t\t" "The target coverage is: " << targetDistrictCoverage * 100.0f << "%\n";
					#endif
					U16_Dist   generateFloorCount { district->minFloors, district->maxFloors };
					F32        randomFloorCount   { static_cast<F32>(generateFloorCount(rng)) };
					BuildingID id                 { generateBuildingID() };
					currentArea += buildingSize;
					for ( auto &tilePosition : potentialLot.value() ) {

						bool isHospital = false;
						if ( not hospitalTable[cellId] ) {
							if (    (tilePosition.x >                   0) and (tilemap->tileAt(tilePosition.x-1, tilePosition.y   ) == Tile::road)
							     or (tilePosition.y >                   0) and (tilemap->tileAt(tilePosition.x,   tilePosition.y-1 ) == Tile::road) 
							     or (tilePosition.x < tilemap->width  - 1) and (tilemap->tileAt(tilePosition.x+1, tilePosition.y   ) == Tile::road) 
							     or (tilePosition.y < tilemap->height - 1) and (tilemap->tileAt(tilePosition.x,   tilePosition.y+1 ) == Tile::road) )
							{
								hospitalTable[cellId] = tilePosition;
								isHospital = true;
							}
						}

						houseTiles.emplace_back();
						auto &houseTile = houseTiles.back();
						buildingIDs[tilemap->index(tilePosition)] = id;
						tilemap->tileAt(tilePosition) = Tile::building;
						// TODO: assign proper meshes when tilesets have been created
						if ( not isHospital ) {
							int randomHouse = rand() % 6; // decides the house
							houseTile.mesh = graphics.getMeshPointer(data(buildingArr[randomHouse]));
							int randomTex = rand() % 3;
							houseTile.setMaterial(graphics.getMaterial(data(textureArr[randomTex])));

							//houseTile.setColor( {.75f, .75f, .75f, 1.0f} );

							F32 randomSize = smallProbabilityDist(rng) < smallProbability ? randomSizeDist(rng) : 1.0f;

							houseTile.setScale({ randomSize * .0322f * config.tileScaleFactor.x,
							                     .015f * config.tileScaleFactor.y + (.25f * randomFloorCount) /* config.buildingFloorHeightFactor * randomFloorCount */,
							                     randomSize * .0322f * config.tileScaleFactor.z });
							houseTile.setPosition({ tilemap->convertTilePositionToWorldPosition(tilePosition) - Vector3(0,2,0) } );
							#ifndef _DEBUG
								btRigidBody *tmp = physics->addBox( btVector3( houseTile.getPosition().x,
																							  houseTile.getPosition().y,
																							  houseTile.getPosition().z ),
																				btVector3( 15.5f * houseTile.getScale().x,
																							  /*15.5f * houseTile.getScale().y*/400,
																							  15.5f * houseTile.getScale().z ),
																				.0f );
								houseTile.setRigidBody( tmp, physics );
								houseTile.getRigidBody()->setFriction(0);
								houseTile.getRigidBody()->forceActivationState(DISABLE_SIMULATION);
							#endif
						}
						else { // is hospital
							// TODO: check which neighbouring tile is the road and rotate the hospital accordingly!
							auto orientation = getHospitalOrientation( tilePosition );
							F32 yRotation = .0f;
							F32 constexpr pi{ 3.14156926535f };
							switch ( orientation ) {
								case Direction::south: { houseTile.setRotation({.0f, pi/2,   .0f}); break; } //  90 klockvis Y
								case Direction::west:  { houseTile.setRotation({.0f, pi,     .0f}); break; } // 180 klockvis Y
								case Direction::north: { houseTile.setRotation({.0f, pi/2*3, .0f}); break; } // 270 klockvis Y
								case Direction::east: { break; } // no need to rotate
								default: assert( false and "BUG! Unaccounted for direction" );
							}
							houseTile.mesh       = graphics.getMeshPointer( "Hospital" );
							houseTile.setMaterial( graphics.getMaterial(    "Hospital" ) );
							houseTile.setScale({ .048f * config.tileScaleFactor.x,
							                     .048f * config.tileScaleFactor.y,
							                     .048f * config.tileScaleFactor.z });
							houseTile.setPosition({ tilemap->convertTilePositionToWorldPosition(tilePosition) } );
							#ifndef _DEBUG_MAP
								btRigidBody *tmp = physics->addBox( btVector3( houseTile.getPosition().x,
																							  houseTile.getPosition().y,
																							  houseTile.getPosition().z ),
																				btVector3( 10.5f * houseTile.getScale().x,
																							  10.5f * houseTile.getScale().y,
																							  10.5f * houseTile.getScale().z ),
																				.0f );
								houseTile.setRigidBody( tmp, physics );
							#endif
						}
						#ifdef _DEBUG_MAP
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
		graphics.addToDrawStatic(&e);

   #ifdef _DEBUG_MAP
	   if (buildingLogs.is_open())
		   buildingLogs.close();
   #endif
}

// basic proto placement algorithm
// TODO: refactor out of Game
Opt<Vector<V2u>>  Map::findValidHouseLot( RNG &rng, U16 cellId, Voronoi const &districtMap, TileMap &map )
 {
   DBG_PROBE(Map::findValidHouseLot);
   U16 const maxTries { 50U }; // TODO: refactor out

	Bounds                cellBounds      { districtMap.computeCellBounds(cellId) };
	U16_Dist              generateOffset  { 0x0, 0xFF };
	U16_Dist              generateX       ( cellBounds.min.x, cellBounds.max.x );
	U16_Dist              generateY       ( cellBounds.min.y, cellBounds.max.y );
	District::Type const *district        { districtLookupTable[cellId] };
	auto      isValidPosition { [&](V2u const &tilePosition ) -> Bool {
	                               return  (districtMap.diagram[districtMap.diagramIndex(tilePosition)] != cellId)
	                                   or (map.tileAt(tilePosition) != Tile::ground)
                                      or (roadDistanceMap[map.index(tilePosition)] < district->maxDistFromRoad ); // TODO: refactor into config or district
	                            } };

	U16_Dist        generateTargetSize { district->minArea, district->maxArea };
	U16      const  targetSize         { generateTargetSize(rng) };

	// find valid starting coordinate:
   U16  currentTry = 0;
	V2u  startPosition{};
	do {
		startPosition.x = generateX( rng );
		startPosition.y = generateY( rng );
		if ( ++currentTry >= maxTries )
			return {};
	} while ( isValidPosition( startPosition ) );

	Vector<V2u>  claimedPositions, candidateSources;

	claimedPositions.reserve( targetSize );
	claimedPositions.push_back( startPosition );

	candidateSources.reserve( targetSize );
	candidateSources.push_back( startPosition );

	while ( claimedPositions.size() != targetSize and candidateSources.size() != 0 ) {
		auto const &sourcePosition = candidateSources[ generateOffset(rng) % candidateSources.size() ];
		// find valid neighbours
		auto  candidateDestinations = map.getCardinallyNeighbouringTilePositions(sourcePosition);
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


TileMap const &Map::getTileMap() const noexcept
{
	return *tilemap;
}

Voronoi const &Map::getDistrictMap() const noexcept
{
   return *districtMap;
}

void  Map::setDistrictColorCoding( Bool useColorCoding ) noexcept
{
	if ( false ) { // TEMP HACK, TODO // useColorCoding ) { 
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
		for ( U16 y = 0;  y < tilemap->height;  ++y ) {
			for ( U16 x = 0;  x < tilemap->width;  ++x ) {
				assert( tilemap->data.size() == districtMap->diagram.size() and "BUG!" );
				auto       &tile      = *groundTiles[ tilemap->index(x, y) ];
				auto        cellIndex = districtMap->diagramIndex(x, y);
				auto        cellId    = districtMap->diagram[ cellIndex ];
				auto const &color     = districtColorTable[ cellId % districtColorTable.size() ];
				tile.setColor( color );
			}
		}
		for ( auto &e : houseTiles ) {
			//assert( e.getPosition().x >= .0f );
			//assert( e.getPosition().z >= .0f );
			auto        cellPosition = tilemap->convertWorldPositionToTilePosition( e.getPosition() );
			auto        cellIndex    = districtMap->diagramIndex( cellPosition.x, cellPosition.y );
			if ( cellIndex < districtMap->diagram.size() ) { // TEMP hack due to coordinate issues
				auto        cellID       = districtMap->diagram[ cellIndex ];
				auto const &color        = districtColorTable[ cellID % districtColorTable.size() ];
				e.setColor( color );
			}
		}
	}
	else {
		static auto const  defaultColor = Vector4{ .0f, .0f, .0f, 1.0f };
		for ( auto& e : houseTiles )
			e.setColor( defaultColor );
	}
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

	U16 searchRadius = config.distanceMapSearchRadius;

	struct  TileDistanceEntry {
		F32   distance;
		V2u   position;
	};

	auto  candidates { Vector<TileDistanceEntry>( (searchRadius*2+1) * (searchRadius*2+1) ) };

	auto manhattanDistance = []( V2u const &a, V2u const &b ) {
		return F32( util::abs<I32>( I32(a.x) - b.x )  +  util::abs<I32>( I32(a.y)- b.y ) );
	};

	auto euclideanDistanceSquared = []( V2u const &a, V2u const &b ) {
		return F32(a.x) * a.x * b.x * b.x * a.y * a.y * b.y * b.y;
	};

	auto euclideanDistance = []( V2u const &a, V2u const &b ) {
		return std::sqrt( F32(a.x) * a.x * b.x * b.x * a.y * a.y * b.y * b.y);
	};

	auto &distance_f = euclideanDistance;

	V2u centerPos;
	for ( centerPos.x = 0;  centerPos.x < static_cast<U16>(config.dimensions.x);  ++centerPos.x ) {
		for ( centerPos.y = 0;  centerPos.y < static_cast<U16>(config.dimensions.y);  ++centerPos.y ) {
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
	}
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
   F32 offsetDistance { config.tileScaleFactor.x / 1.6f }; // TODO: tune!
   auto orientation = getHospitalOrientation( hospitalTilePos );
   auto result = tilemap->convertTilePositionToWorldPosition( hospitalTilePos );
   switch ( orientation ) { 
      case Direction::south: result += Vector3(  .0f, .0f,  -offsetDistance ); break;
      case Direction::west:  result += Vector3( -offsetDistance, .0f, .0f  ); break;
      case Direction::east:  result += Vector3(  offsetDistance, .0f, .0f  ); break;
      case Direction::north: result += Vector3(  .0f, .0f, offsetDistance ); break;
   }
   return result;
}

District::Type const *Map::districtAt( U32 x, U32 y ) const noexcept {
	auto tileIdx = districtMap->diagramIndex(x,y);
	auto cellIdx = districtMap->diagram[ tileIdx ];
	return districtLookupTable[ cellIdx ];
}






// Tile neighbours bit layout:
//
//  7     0     1
//    NW  N  NE
//
//  6 W   c   E 2
//
//    SW  S  SE 
//  5     4     3
//
// c is not included (it's intrinsic based off of the currently processed tiled)

// Wraps a tile's type and encodes the possible existence of neighbouring road tiles in the 8 eight directions into bits.
struct TileInfo {
   union {
		U8 bitmap  { 0x00 }; // <- encoding byte; default unset
		struct {             // <- individual bit access interface
			U8 n  : 1,
			   ne : 1,
			   e  : 1,
			   se : 1,
			   s  : 1,
			   sw : 1,
			   w  : 1,
			   nw : 1;
		};
	} roadmap, housemap, concreteMap;
	Vector3  origin;
	Tile     tileType;
	District::Type const *districtType;
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
				auto districtRoadsHaveSidewalk = []( District::Type const *d ) constexpr {
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
static F32 constexpr transitionOffsetY { -1.48f };
static F32 constexpr sidewalkOffsetY   { -1.47f };

//1x1
//1x2 / 2x2
//2x2
//3x2 / 2x3
//4x4
//
//
//_00000000_50
//0/1
//
//Houses/Prefabs/4x4_namn_/mesh.bin
// struct PrefabHouse {
// 	GameObject house;
// 	Vector2u   
// };
/*
struct HouseTileset {
	String                        name;
	F32                           floorHeight;
	Vector<District::Type const*>	validInDistricts;
};

struct CompositeHouse {
	GameObject  roof, walls, windows;
	Bounds      bounds;
};

struct SingleTileHouse {
	GameObject  house;
	V2u         tilePos;
};

struct MultiTileHouse {
	Vector<GameObject>  tiles;
	
	Bounds              bounds;
	U8                  floorLayout;
};

struct HouseData {
	Vector<SingleTileHouse>  singles;
	Vector<MultiTileHouse>   multis;
	Vector<CompositeHouse>   composites;
};


Vector<Vector<GameObject>> Map::instantiateHousesAaModels() noexcept {
	// multi-tile houses:
	auto instantiatePart = [&]( std::string_view name, Vector3 const &pos, F32 deg=.0f, F32 yOffset=baseOffsetY, Bool hasNormal=true, Bool noShadowcasting=false ) {
		models.push_back( std::make_unique<GameObject>() );
      auto &model     =  *models.back();
		model.mesh      =   graphics.getMeshPointer( "Tiles/Quad_SS" );
		model.setTexture(   graphics.getTexturePointer(name.data()) );
		if ( hasNormal )
			model.setNormalMap( graphics.getTexturePointer( (std::string(name)+"_nor").c_str() ) );
		if ( deg > 1.0f )
			model.setRotation({ .0f, util::degToRad(deg), .0f });
		model.setPosition( pos + Vector3{.0f, yOffset, .0f} );
		model.setColor({ .0f, .0f, .0f, .0f });
		model.setScale({ tilemap->config.tileScaleFactor.x, 1.0f, tilemap->config.tileScaleFactor.z });
		model.setSpotShadow( noShadowcasting );
		model.setSunShadow(  noShadowcasting );
	};
	for ( auto &building : buildings ) {
		for ( auto &pos : building.positions ) {
			U8 tilemask = 0x00
			if ( (pos.y-1 < tilemap->height) and (buildingIDs[tilemap->index(pos.x, pos.y-1)]==building.id) )
				tilemask += 1 << 0; // N
			if ( (pos.y-1 < tilemap->height) and (pos.x+1 < tilemap->width)
				  and (buildingIDs[tilemap->index(pos.x+1, pos.y-1)]==building.id) )
				tilemask += 1 << 1; // NE
			if ( (pos.x+1 < tilemap->width)
				  and (buildingIDs[tilemap->index(pos.x+1, pos.y)]==building.id) )
				tilemask += 1 << 2; // E
			if ( (pos.y+1 < tilemap->height) and (pos.x+1 < tilemap->width)
				  and (buildingIDs[tilemap->index(pos.x+1, pos.y-1)]==building.id) )
				tilemask += 1 << 3; // SE
			if ( (pos.y+1 < tilemap->height)
				  and (buildingIDs[tilemap->index(pos.x, pos.y+1)]==building.id) )
				tilemask += 1 << 4; // S
			if ( (pos.y+1 < tilemap->height) and (pos.x-1 < tilemap->width)
				  and (buildingIDs[tilemap->index(pos.x-1, pos.y-1)]==building.id) )
				tilemask += 1 << 5; // SW
			if ( (pos.x-1 < tilemap->width)
				  and (buildingIDs[tilemap->index(pos.x-1, pos.y)]==building.id) )
				tilemask += 1 << 6; // W
			if ( (pos.y-1 < tilemap->height) and (pos.x-1 < tilemap->width)
				  and (buildingIDs[tilemap->index(pos.x-1, pos.y-1)]==building.id) )
				tilemask += 1 << 1; // NW

			for ( auto q=0; q<4; ++q ) { // quadrants
				U8 quadmask = tilemask >> (2 * q);
				if ( quadmask & 0b101 == 0x00 ) {
					instantiateModel( "/Buildings/"+building.tileset+"/f_oc", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, .0f );
					for ( auto currFloor = 0; currFloor < building.numFloors; ++currFloor )
						instantiateModel( "/Buildings/"+building.tileset+"/w_oc", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, currFloor*building.floorHeight );
					instantiateModel( "/Buildings/"+building.tileset+"/r_oc", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, building.numFloors*building.floorHeight );
				}
				else if ( quadmask == 0b101 ) {
					 instantiateModel( "/Buildings/"+building.tileset+"/f_ic", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, .0f );
					for ( auto currFloor = 0; currFloor < building.numFloors; ++currFloor )
						instantiateModel( "/Buildings/"+building.tileset+"/w_ic", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, currFloor*building.floorHeight );
					instantiateModel( "/Buildings/"+building.tileset+"/r_ic", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, building.numFloors*building.floorHeight );
				}
				else if ( quadmask == 0b100 ) {
					instantiateModel( "/Buildings/"+building.tileset+"/f_sa", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, .0f );
					for ( auto currFloor = 0; currFloor < building.numFloors; ++currFloor )
						instantiateModel( "/Buildings/"+building.tileset+"/w_sa", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, currFloor*building.floorHeight );
					instantiateModel( "/Buildings/"+building.tileset+"/r_sa", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, building.numFloors*building.floorHeight );
				}
				else if ( quadmask == 0b001 ) {
					instantiateModel( "/Buildings/"+building.tileset+"/f_sb", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, .0f );
					for ( auto currFloor = 0; currFloor < building.numFloors; ++currFloor )
						instantiateModel( "/Buildings/"+building.tileset+"/w_sb", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, currFloor*building.floorHeight );
					instantiateModel( "/Buildings/"+building.tileset+"/r_sb", tilemap->convertTilePositionToWorldPosition(x,y), 45.0f*q, building.numFloors*building.floorHeight );
				}
				else if ( quadmask = 0b111 ) {
					instantiateModel( "/Buildings/"+building.tileset+"/r_m", tilemap->convertTilePositionToWorldPosition(x,y), .0f, building.numFloors*building.floorHeight );
				}
			}
		}
	}


}
*/
Vector<UPtr<GameObject>> Map::instantiateTilesAsModels() noexcept
{
	Vector<UPtr<GameObject>> models;
	models.reserve( tilemap->data.size() * 3 );// TODO: use worst case scenario: 6 quads per tile?
	auto instantiatePart = [&]( std::string_view name, Vector3 const &pos, F32 deg=.0f, F32 yOffset=baseOffsetY, Bool hasNormal=true, Bool noShadowcasting=false ) {
		models.push_back( std::make_unique<GameObject>() );
      auto &model     =  *models.back();
		model.mesh      =   graphics.getMeshPointer( "Tiles/Quad_SS" );
		model.setTexture(   graphics.getTexturePointer(name.data()) );
		if ( hasNormal )
			model.setNormalMap( graphics.getTexturePointer( (std::string(name)+"_nor").c_str() ) );
		if ( deg > 1.0f )
			model.setRotation({ .0f, util::degToRad(deg), .0f });
		model.setPosition( pos + Vector3{.0f, yOffset, .0f} );
		model.setColor({ .0f, .0f, .0f, .0f });
		model.setScale({ tilemap->config.tileScaleFactor.x, 1.0f, tilemap->config.tileScaleFactor.z });
		model.setSpotShadow( noShadowcasting );
		model.setSunShadow(  noShadowcasting );
	};
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
				if ( maskedMap == pred4Way )
					instantiatePart( "Tiles/road_marker_4way", e.origin, .0f, markerOffsetY, false );
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
						instantiatePart( "Tiles/road_marker_deadend_n", e.origin, 45.0f*d, markerOffsetY, false );
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
						instantiatePart( "Tiles/road_2file_deadend_n", e.origin, 45.0f*d, markerOffsetY );
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
	return models;
}

Biome Map::getBiome() const noexcept {
	return biome;
}
