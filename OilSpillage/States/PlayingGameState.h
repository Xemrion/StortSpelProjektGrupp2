#pragma once 

#include <array>
#include "GameState.h"
#include "../game.h"
#include "../ImGui/imgui.h"
#include "../vehicle.h"
#include "../AI/Actor.h"
#include "../Road Network/RoadNet.h"
#include "../DynamicCamera.h"
#include "../PG/MapConfig.hpp"
#include "../PG/Map.hpp"
#include "../PG/Walker.hpp"
#include "../PG/Map.hpp"
#include "../PG/Voronoi.hpp"
#include "../PG/District.hpp"
#include "../UI/UserInterface.h"

class PlayingGameState : public GameState {
friend class Game;
public:
	enum  Menu { MENU_PLAYING,
	             MENU_PAUSED,
	             MENU_OPTIONS,
	             MENUCOUNT };

	             PlayingGameState();
	virtual     ~PlayingGameState();
	void         update(float delta);
	const float &getTimeRef() const noexcept;
	float        getTime() const noexcept;
	void         setTime(float) noexcept;
	void         changeTime(float delta) noexcept;
	void         setCurrentMenu(Menu);
	Vehicle     *getPlayer() const;

private:
	friend class Game;
	int                             currentMenu;
	bool                            isUsingManhattanDistance { true };
	float                           cameraDistance           { 25   };
	float                           time;
	MapConfig                       config;
	Vector3                         startPos;
	Graphics                       &graphics;
	std::unique_ptr<AStar>          aStar;
	std::unique_ptr<Map>            map;
	std::unique_ptr<Actor>          aiObject;
	std::unique_ptr<LightList>      lightList;
	std::unique_ptr<Vehicle>        player;
	std::unique_ptr<Voronoi>        districtMap;
	std::unique_ptr<DynamicCamera>  camera;
	std::unique_ptr<UserInterface>  menues[MENUCOUNT];
	std::unique_ptr<RoadNetwork>    testNetwork;
	std::vector<CinematicPos>       points;
	std::vector<GameObject>         districtMarkers;
	std::vector<GameObject>         roadTiles;
	std::vector<GameObject>         houseTiles;
	SpotLight                      *playerLight;

	//Bullet
	std::unique_ptr<Physics>		physics;
	std::unique_ptr<GameObject>		buildingTest;

	int                             addNrOfParticles  {     2 };
	int                             lifeTime          {     2 };
	float                           timerForParticle  {   .0f };
	float                           vectorFieldPower  {  1.0f };
	float                           vectorFieldSize   {  1.0f };
	float                           randomPosPower    {  1.0f };
	float                           size1             { .020f };
	float                           size2             { .025f };
	float                           colors  [4]       {};
	float                           colors2 [4]       {};
	float                           colors3 [4]       {};
	float                           colors4 [4]       {};
	Vector4                         colorsP [4]       { Vector4( 1.0f, 1.0f, .0f, 1.0f ),
	                                                    Vector4( 1.0f,  .0f, .0f, 1.0f ),
	                                                    Vector4(  .0f,  .0f, .0f, 1.0f ),
	                                                    Vector4(  .1f,  .1f, .1f, 1.0f )  };

	void           generateBuildings( MapConfig const &, RNG & );
	void           ImGui_ProcGen();
	void           ImGui_Driving();
	void           ImGui_Particles();
	void           initiateAStar();
	V2u            generateMap( MapConfig const & );
	V2u            generateRoadPosition(MapConfig const &, Map const &, RNG & ) const noexcept;
	void           ImGui_Camera();
	void           setDistrictColors(bool useColorCoding) noexcept;
	Opt<Vec<V2u>>  find_valid_house_lot( RNG &, U16 cell_id, Voronoi const &district_map, Map &, Vec<District> const &districtTable );
};
