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
	//Particles config
	Vector4 colorsP[4] =
	{
		Vector4(1.0f,1.0f,0.0f,1.0f),
		Vector4(1.0f,0.0f,0.0f,1.0f),
		Vector4(0.0f,0.0f,0.0f,1.0f),
		Vector4(0.1f,0.1f,0.1f,1.0f)
	};
	float colors[4];
	float colors2[4];
	float colors3[4];
	float colors4[4];
	float size1 = 0.02f;
	float size2 = 0.025f;
	int addNrOfParticles = 2;
	int lifeTime = 2;

	void           generateMap( Config const & );
   void           generateBuildings( Config const &, RNG & );
   void           ImGui_ProcGen();
   void           ImGui_Driving();
   void           ImGui_Particles();
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
