#pragma once

#include "../graphics.h"
#include "defs.hpp"
#include "TileMap.hpp"
#include "MapConfig.hpp"
#include "RoadGenerator.hpp"
#include "Voronoi.hpp"
#include "District.hpp"

class Map {
public:
   Map( Graphics &, MapConfig const & );
  ~Map() noexcept;

   Graphics        &graphics;
   MapConfig const &config;
   V2u              startPositionInTileSpace;
   UPtr<TileMap>    tilemap;
   UPtr<Voronoi>    districtMap;
   Vec<GameObject>  districtMarkers;
   Vec<GameObject>  roadTiles;
   Vec<GameObject>  houseTiles;

   V2u              generateRoadPositionInTileSpace(  RNG & ) const noexcept;
   Vector3          generateRoadPositionInWorldSpace( RNG & ) const noexcept;
   Void             setDistrictColorCoding ( Bool useColorCoding ) noexcept;
   V2u              getStartPositionInTileSpace()  const noexcept;
   Vector3          getStartPositionInWorldSpace() const noexcept;
   TileMap const   &getTileMap() const noexcept;

private:
   Void             generateDistricts();
   Void             generateRoads();
   Void             generateBuildings();
   Opt<Vec<V2u>>    findValidHouseLot( RNG &, U16 districtCellID, Voronoi const &, TileMap &, Vec<District> const &districtTable );
};
