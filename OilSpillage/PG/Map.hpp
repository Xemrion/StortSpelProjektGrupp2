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
	Map( Graphics &, MapConfig const &, Physics * );
	~Map() noexcept;

	MapConfig const &config;

	V2u              generateRoadPositionInTileSpace(  RNG & ) const noexcept;
	Vector3          generateRoadPositionInWorldSpace( RNG & ) const noexcept;
	void             setDistrictColorCoding ( Bool useColorCoding ) noexcept;
	V2u              getStartPositionInTileSpace()  const noexcept;
	Vector3          getStartPositionInWorldSpace() const noexcept;
	TileMap const   &getTileMap() const noexcept;
	Voronoi const   &getDistrictMap() const noexcept;
// TODO: refactor out
	Vector<Opt<V2u>> const &getHospitalTable() const noexcept;

private:
	void             generateDistricts();
	void             generateRoads();
	void             generateBuildings();
	Opt<Vector<V2u>> findValidHouseLot( RNG &, U16 districtCellID, Voronoi const &, TileMap &, Vector<District> const &districtTable );

	Graphics              &graphics;
	V2u                    startPositionInTileSpace;
	UPtr<TileMap>          tilemap;
	UPtr<Voronoi>          districtMap;
	Vector<GameObject>     districtMarkers;
	Vector<GameObject>     roadTiles;
	Vector<GameObject>     houseTiles;
	Physics * const        physics;
	// TODO: refactor out:
	using DistrictID = U16;
	using BuildingID = U16;                    // 0 = unused tile
	BuildingID          nextBuildingID { 1U }; // 1 = first valid ID
	Vector<BuildingID>  buildingIDs;
	Vector<U16>         roadDistanceMap;
	BuildingID          generateBuildingID()  noexcept;
	void                generateDistanceMap() noexcept;
	Vector<Opt<V2u>>    hospitalTable;
};
