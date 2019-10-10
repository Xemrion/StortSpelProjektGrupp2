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
#include "../UI/UserInterface.h"

class PlayingGameState : public GameState
{
public:
	enum Menu
	{
		MENU_PLAYING,
		MENU_PAUSED,
		MENU_OPTIONS,
		MENUCOUNT
	};
private:
	friend class Game;
	std::unique_ptr<UserInterface>  menues[MENUCOUNT];
	int                             currentMenu;
	bool                            isUsingManhattanDistance = true;
	float                           cameraDistance = 25;
	float                           time;
	Config                          config;
	Graphics                       &graphics;
	std::unique_ptr<Map>            map;
	std::unique_ptr<Actor>          aiObject;
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
	float timerForParticle = 0.0f;
	float vectorFieldPower = 1.0f;
	float vectorFieldSize = 1.0f;
	float randomPosPower = 1.0f;

	void           generateMap( Config const & );
   void           generateBuildings( Config const &, RNG & );
   void           ImGui_ProcGen();
   void           ImGui_Driving();
   void           ImGui_Particles();
   Opt<Vec<V2u>>  find_valid_house_lot( RNG &, U16 cell_id, Voronoi const &district_map, Map &, Vec<District> const &district_tbl );
   void           toggleDistrictColors() noexcept;
	void           initiateAStar();
	std::unique_ptr<AStar>          aStar;

	V2u				generateMap( Config const & );
	V2u				generateRoadPosition(Config const& config, Map const& map, RNG& rng) const noexcept;
	void			generateBuildings( Config const &, RNG & );
	void			ImGui_ProcGen();
	void			ImGui_Driving();
	void			ImGui_Camera();
	Opt<Vec<V2u>>	find_valid_house_lot( RNG &, U16 cell_id, Voronoi const &district_map, Map &, Vec<District> const &district_tbl );
	void			setDistrictColors(bool useColorCoding) noexcept;
	void			initiateAStar();

public:
					PlayingGameState();
	virtual        ~PlayingGameState();
	void			update(float deltaTime);
	const float	   &getTimeRef() const noexcept;
	float			getTime() const noexcept;
	void			setTime(float time) noexcept;
	void			changeTime(float timeDiff) noexcept;
	void			setCurrentMenu(Menu menu);
	Vehicle		   *getPlayer() const;
};
