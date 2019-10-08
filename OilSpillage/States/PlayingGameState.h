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

class PlayingGameState : public GameState
{
private:
	friend class Game;
   Config                          config;
	Graphics                       &graphics;
	std::unique_ptr<Map>            map;
	std::unique_ptr<Actor>       aiObject;
	std::vector<GameObject>         districtMarkers;
	std::vector<GameObject>         roadTiles;
   std::vector<GameObject>         houseTiles;
	std::unique_ptr<LightList>      lightList;
	std::unique_ptr<Vehicle>        player;
   std::unique_ptr<Voronoi>        districtMap;
	std::unique_ptr<DynamicCamera>  camera;
	std::vector<CinematicPos>       points;
	SpotLight                      *playerLight = nullptr;
	std::unique_ptr<RoadNetwork>    testNetwork;
	AStar*							aStar;
	void           generateMap( Config const & );
   void           generateBuildings( Config const &, RNG & );
   void           ImGui_ProcGen();
   void           ImGui_Driving();
   Opt<Vec<V2u>>  find_valid_house_lot( RNG &, U16 cell_id, Voronoi const &district_map, Map &, Vec<District> const &district_tbl );
   void           toggleDistrictColors() noexcept;
	void           initiateAStar();
public:
	PlayingGameState();
	virtual ~PlayingGameState();
	void init();
   virtual void cleanUp() {}
	void update(float deltaTime);
};
