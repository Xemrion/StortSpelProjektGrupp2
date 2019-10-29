#include "Map.hpp"
#include "Profiler.hpp"
#include "utils.hpp"

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

Map::Map( Graphics &graphics, MapConfig const &config, Physics *physics ):
	graphics        ( graphics                                                                                      ),
	physics         ( physics                                                                                       ),
	config          ( config                                                                                        ),
	tilemap         ( std::make_unique<TileMap>( config )                                                           ),
	districtMarkers ( static_cast<Size>( config.districtCellSide) * config.districtCellSide                         ),
	roadTiles       ( static_cast<Size>( config.dimensions.x)     * config.dimensions.y                             ),
   roadDistanceMap ( config.dimensions.x * config.dimensions.y                                                     ),
	buildingIDs     ( config.dimensions.x * config.dimensions.y                                                     ),
	hospitalTable	 ( config.dimensions.x / config.districtCellSide * config.dimensions.y / config.districtCellSide )
{
	DBG_PROBE(Map::Map);
	// TODO: generate water etc
	generateDistricts();
	generateRoads();
	generateRoadDistanceMap();
	generateBuildings();
}


Map::~Map() noexcept
{
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
void  Map::generateRoads()
{
	DBG_PROBE(Map::generateRoads);
	RNG      rng{ RD()() };
	I32_Dist generateSeed{};
	rng.seed(config.seed);
	
	MapConfig  roadConfig { config };
	while (true) { // TODO: add MAX_TRIES?
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
	
	roadTiles = tilemap->loadAsModels(graphics); // GameObject instantiation
	for ( U16 y = 0;  y < tilemap->height;  ++y ) {
		for ( U16 x = 0;  x < tilemap->width;  ++x ) {
			auto &tile = roadTiles[tilemap->index(x,y)];
			tile.setScale(Vector3{ .0005f * config.tileScaleFactor.x,
			                       .0005f * config.tileScaleFactor.y,
			                       .0005f * config.tileScaleFactor.z }); // TODO: scale models instead
			tile.setNormalMap(graphics.getTexturePointer("brickwallnormal"));
			graphics.addToDraw( &tile , true );
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
   if constexpr ( isDebugging ) {
	   // display noise centers:
	   districtMarkers.reserve( districtMap->noise.size() );
	   for ( auto const &cellCentre : districtMap->noise ) {
			districtMarkers.emplace_back();
			auto &marker = districtMarkers.back();
			marker.mesh  = graphics.getMeshPointer("Cube");
			marker.setColor({ 1, 0,  0, 1 });
			marker.setScale({ .2f, 4.0f, .2f });
			marker.setPosition({ tilemap->convertTilePositionToWorldPosition( static_cast<U16>(cellCentre.x),
			                                                                  static_cast<U16>(cellCentre.y)) });
			graphics.addToDraw(&marker, true);
	   }
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

void  Map::generateBuildings( )
{
	DBG_PROBE(Map::generateBuildings);

	U16 const maxTries { 25U }; // TODO: refactor out

	RNG  rng{ RD()() };
	rng.seed( config.seed );
	F32_Dist randomSizeDist { .6f, .7f };
	F32 constexpr smallProbability{ .20f };
	F32_Dist smallProbabilityDist {};

	//Array of possible buildings
	std::string buildingArr[4];
	buildingArr[0] = "Houses/testHouse";
	buildingArr[1] = "Houses/testHouse2";
	buildingArr[2] = "Houses/testHouse3";
	buildingArr[3] = "Houses/testHouse4";
	
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
		districtLookupTable[i] = cellIdToDistrictEnum(i);

	for ( U16 cellId = 0;  cellId < districtMap->noise.size();  ++cellId) {
		District    districtType{ districtLookupTable[cellId] };
		Size        currentArea{ 0 };
		Size const  cellArea{ districtMap->computeCellRealEstateArea(cellId,*tilemap) };
		
		#ifdef _DEBUG
		   U64  currrentDistrictBuildingCount { 0 };
		   buildingLogs << "\n\t" "Generating buildings for district #"
		                << cellId << " (TYPE: \"" << stringify(districtType) << "\")\n";
		#endif
		
		if ( (cellArea != 0) ) { //and (districtType != District::park) ) {
			F32  const  targetDistrictCoverage         { District_getBuildingDensity(districtType) };
			auto        computeCurrentDistrictCoverage { [&currentArea, cellArea]() {
			                                                return F32(currentArea) / F32(cellArea);
			                                             } };
         U16 currentTries { 0U };
			while ( computeCurrentDistrictCoverage() < targetDistrictCoverage and ++currentTries < maxTries ) {
				auto potentialLot = findValidHouseLot(rng, cellId, *districtMap, *tilemap, districtLookupTable);
				if ( potentialLot ) {
					if ( not hospitalTable[cellId] )
						hospitalTable[cellId] = potentialLot.value()[0];
               currentTries = 0; // reset try counter
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
						buildingIDs[tilemap->index(tilePosition)] = id;
						tilemap->tileAt(tilePosition) = Tile::building;
						// TODO: assign proper meshes when tilesets have been created
						int randomHouse = rand() % 4; //decides the house
						houseTile.mesh = graphics.getMeshPointer(data(buildingArr[randomHouse]));
						//houseTile.setColor( {.75f, .75f, .75f, 1.0f} );

						F32 randomSize = smallProbabilityDist(rng) < smallProbability ? randomSizeDist(rng) : 1.0f;

						houseTile.setScale({ randomSize * .0322f * config.tileScaleFactor.x,
						                     .015f * config.tileScaleFactor.y + (.25f * randomFloorCount) /* config.buildingFloorHeightFactor * randomFloorCount */,
											 randomSize * .0322f * config.tileScaleFactor.z });
						houseTile.setPosition({ tilemap->convertTilePositionToWorldPosition(tilePosition) - Vector3(0,1,0) } );
				  #ifndef _DEBUG
						btRigidBody *tmp = physics->addBox( btVector3( houseTile.getPosition().x,
						                                               houseTile.getPosition().y,
						                                               houseTile.getPosition().z ),
						                                    btVector3( 15.5f * houseTile.getScale().x,
						                                               15.5f * houseTile.getScale().y,
																       15.5f * houseTile.getScale().z ),
						                                    .0f );
						houseTile.setRigidBody( tmp, physics );
				  #endif
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
Opt<Vector<V2u>>  Map::findValidHouseLot( RNG &rng, U16 cellId, Voronoi const &districtMap, TileMap &map, Vector<District> const &districtLookUpTable )
 {
   DBG_PROBE(Map::findValidHouseLot);
   U16 const maxTries { 50U }; // TODO: refactor out

	Bounds    cellBounds      { districtMap.computeCellBounds(cellId) };
	U16_Dist  generateOffset  { 0x0, 0xFF };
	U16_Dist  generateX       { cellBounds.min.x, cellBounds.max.x };
	U16_Dist  generateY       { cellBounds.min.y, cellBounds.max.y };
	District const  districtType       { districtLookUpTable[cellId] }; // TODO: refactor? (and param)
	auto      isValidPosition { [&](V2u const &tilePosition ) -> Bool {
	                               return  (districtMap.diagram[districtMap.diagramIndex(tilePosition)] != cellId)
	                                   or (map.tileAt(tilePosition) != Tile::ground)
                                      or (roadDistanceMap[map.index(tilePosition)] > District_getBuildingMaxDistanceFromRoad(districtType)); // TODO: refactor into config or district
	                            } };

	U16_Dist        generateTargetSize { District_getBuildingMinArea(districtType), District_getBuildingMaxArea(districtType) };
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
		for ( U16 y = 0;  y < tilemap->height;  ++y ) {
			for ( U16 x = 0;  x < tilemap->width;  ++x ) {
				assert( tilemap->data.size() == districtMap->diagram.size() and "BUG!" );
				auto       &tile      = roadTiles[ tilemap->index(x, y) ];
				auto        cellIndex = districtMap->diagramIndex(x, y);
				auto        cellId    = districtMap->diagram[ cellIndex ];
				auto const &color     = districtColorTable[ cellId % districtColorTable.size() ];
				tile.setColor( color );
			}
		}
		for ( auto &e : houseTiles ) {
			auto        cellPosition = tilemap->convertWorldPositionToTilePosition( e.getPosition() );
			auto        cellIndex    = districtMap->diagramIndex( cellPosition.x, cellPosition.y );
			auto        cellID       = districtMap->diagram[ cellIndex ];
			auto const &color        = districtColorTable[ cellID % districtColorTable.size() ];
			e.setColor( color );
		}
	}
	else {
		static auto const  defaultColor = Vector4{ .5f, .5f, .5f, 1.0f };
		for ( auto &e : roadTiles )
			e.setColor( defaultColor );
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

	auto &distance_f = manhattanDistance;

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
					roadDistanceMap[tilemap->index(centerPos)] = foundRoadDistance.value_or( distance_f({0,0}, {searchRadius+1, searchRadius+1}) );
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
