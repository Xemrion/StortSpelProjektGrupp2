#pragma once 

#include <array>
#include "GameState.h"
#include "../game.h"
#include "../ImGui/imgui.h"
#include "../vehicle.h"
#include "../AI/Actor.h"
#include "../Road Network/RoadNet.h"
#include "../DynamicCamera.h"
#include "../PG/config.hpp"
#include "../PG/Map.hpp"
#include "../PG/Walker.hpp"
#include "../PG/Map.hpp"
#include "../PG/Voronoi.hpp"
#include "../PG/District.hpp"

class PlayingGameState : public GameState {
friend class Game;
public:
	               PlayingGameState();
	virtual       ~PlayingGameState();
	Void           init();
	Void           cleanUp();
	Void           update(F32 deltaTime);
	F32 const     &getTimeRef()   const noexcept;
	F32            getTime()      const noexcept;
	Void           setTime(F32 time)    noexcept;
	Void           addTime(F32 time)    noexcept; // TODO: slå ihop addTime och removeTime?
	Void           removeTime(F32 time) noexcept;

private:
   V2u            generateMap( Config const & ); // returns tile XY coord of start tile (Map::tile_xy_to_world_pos() can convert it world space XYZ)
   Void           generateBuildings( Config const &, RNG & );
   Void           ImGui_ProcGen();
   Void           ImGui_Driving();
   Void           ImGui_Camera();
   Opt<Vec<V2u>>  find_valid_house_lot( RNG &, U16 cell_id, Voronoi const &district_map, Map &, Vec<District> const &district_tbl );
   V2u            generateRoadPosition( Config const &, Map const &, RNG & ) const noexcept;
   Void           setDistrictColors(bool useColorCoding) noexcept;
	Void           initiateAStar();

   Bool                 isUsingManhattanDistance = true;
   Config               config;
   F32                  cameraDistance = 25;
   F32                  time;
	Graphics            &graphics;
   SpotLight           *playerLight = nullptr;
	UPtr<LightList>      lightList;
	UPtr<Vehicle>        player;
   UPtr<Voronoi>        districtMap;
	UPtr<DynamicCamera>  camera;
   UPtr<Map>            map;
	UPtr<Actor>          aiObject;
	UPtr<RoadNetwork>    testNetwork;
	UPtr<AStar>          aStar;
   Vec<GameObject>      districtMarkers;
	Vec<GameObject>      roadTiles;
   Vec<GameObject>      houseTiles;
   Vec<CinematicPos>    points;
};
