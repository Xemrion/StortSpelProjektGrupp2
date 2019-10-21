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
   TileMap*    tilemap;
   UPtr<Voronoi>    districtMap;
   Vector<GameObject>  districtMarkers;
   Vector<GameObject>  roadTiles;
   Vector<GameObject>  houseTiles;

   V2u              generateRoadPositionInTileSpace(  RNG & ) const noexcept;
   Vector3          generateRoadPositionInWorldSpace( RNG & ) const noexcept;
   void             setDistrictColorCoding ( Bool useColorCoding ) noexcept;
   V2u              getStartPositionInTileSpace()  const noexcept;
   Vector3          getStartPositionInWorldSpace() const noexcept;
   TileMap const   &getTileMap() const noexcept;
   TileMap*			getTileMapPtr();

private:
   void             generateDistricts();
   void             generateRoads();
   void             generateBuildings();
   Opt<Vector<V2u>>    findValidHouseLot( RNG &, U16 districtCellID, Voronoi const &, TileMap &, Vector<District> const &districtTable );
};
