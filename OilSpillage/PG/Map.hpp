#pragma once

#include "../graphics.h"
#include "defs.hpp"
#include "TileMap.hpp"
#include "MapConfig.hpp"
#include "RoadGenerator.hpp"
#include "Voronoi.hpp"
#include "District.hpp"
#include "../UI/Playing/UIPlaying.h"
#include "Biome.hpp"

struct HouseTileset {
	String                  name;
	F32                     floorHeight;
	Vector<District::Enum>	validInDistricts;
};

extern Vector<HouseTileset> const houseTilesets;

auto getCompatibleTilesets( District::Enum d ) noexcept;

struct MultitileLayout {
   U32        width, length; // size() => width*length
   Vector<U8> floors;        // floors[y * width +x]
};

extern Vector<MultitileLayout> multitileLayouts;

void loadMultitileLayouts( String const &path );

Opt<const MultitileLayout *> getMultitileLayout( District::Enum, RNG & ) noexcept;

// struct PrefabHouse {
// 	TODO
// };

struct CompositeHouse {
	GameObject  walls, windows, roof;
	Bounds      bounds;
};

struct SingleTileHouse {
	GameObject   object;
};

struct MultiTileHouse {
	Vector<GameObject> parts;
	Vector<GameObject> hitboxes;
	Bounds             bounds;
};

struct HouseGenData {
	Vector<SingleTileHouse>  singles;
	Vector<MultiTileHouse>   multis;
	Vector<CompositeHouse>   composites;
};

class Map {
public:
	Map( Graphics &, MapConfig const &, Physics * );
	~Map() noexcept;
	Map()                          = delete;
	Map( Map       && )            = delete;
	Map( Map const  & )            = delete;
	Map& operator=( Map       && ) = delete;
	Map& operator=( Map const  & ) = delete;

	MapConfig const &config;

	V2u                        generateRoadPositionInTileSpace(  RNG & ) const noexcept;
	Vector3                    generateRoadPositionInWorldSpace( RNG & ) const noexcept;
	V2u                        getStartPositionInTileSpace()  const noexcept;
	Vector3                    getStartPositionInWorldSpace() const noexcept;
	TileMap const &            getTileMap() const noexcept;
	Voronoi const &            getDistrictMap() const noexcept;
// TODO: refactor out
	Vector<Opt<V2u>> const &   getHospitalTable() const noexcept;
	Opt<V2u>                   getNearestFoundHospitalTilePos( Vector3 const &sourceWorldPos, UIPlaying const & ) const noexcept;
	Direction                  getHospitalOrientation(   V2u const hospitalTilePos ) const noexcept;
	Vector3                    getHospitalFrontPosition( V2u const hospitalTilePos ) const noexcept;
	District::Enum             districtAt( U32 x, U32 y ) const noexcept;
	Biome                      getBiome() const noexcept;
private:
	void                       generateDistricts();
	void                       generateRoads();
	void                       generateBuildings();
	void                       generateMultitileBuildings();
	Opt<Lot>                   findRandomLot( U16 districtId ) noexcept;
	Opt<Lot>                   findFixedLot( U16 districtId, U32 width, U32 length, Vector<Bool> const &&layout ) noexcept;
	Vector<UPtr<GameObject>>   instantiateTilesAsModels() noexcept;
	void                       instantiateHousesAsModels() noexcept;
	MultiTileHouse             instantiateMultitileHouse( V2u const &nw, MultitileLayout const &, HouseTileset const & ) const noexcept;
	//void                       generateTransitions() noexcept;
	Graphics &               graphics;
	V2u                      startPositionInTileSpace;
	UPtr<TileMap>            tilemap;
	UPtr<Voronoi>            districtMap;
	Vector<District::Enum>   districtLookupTable;
	Vector<UPtr<GameObject>> groundTiles;
	Physics * const          physics;
	// TODO: refactor out:
	using DistrictID = U16;
	using BuildingID = U16;                         // 0 = unused tile
	BuildingID               nextBuildingID { 1U }; // 1 = first valid ID
	Vector<BuildingID>       buildingIDs;
	Vector<F32>              roadDistanceMap;
	BuildingID               generateBuildingID()      noexcept;
	void                     generateRoadDistanceMap() noexcept;
	Vector<Opt<V2u>>         hospitalTable;
	Biome                    biome;
	HouseGenData             houses;
	RNG                      rng; // TODO, instantiate in ctor
};
