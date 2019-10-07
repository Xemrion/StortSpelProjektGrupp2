#pragma once

#include <array>
#include "window.h"
#include "ImGui/imgui.h"
#include "graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "States/GameState.h"
#include "vehicle.h"
#include "DynamicCamera.h"
#include "Road Network/RoadNet.h"
#include "PG/District.hpp"
#include "PG/Voronoi.hpp"
#include "PG/Map.hpp"
#include "PG/defs.hpp"
#include "AI/Actor.h"
#include "UI/UIMainMenu.h"

enum State
{
	STATE_MENU,
	STATE_PLAYING,
	STATECOUNT
};

class Game {
public:
	static GameState* getCurrentState();
	static void setState(State state);
	inline static Graphics& getGraphics() noexcept { return graphics; };
private:
	static Graphics graphics;
	static     std::unique_ptr<GameState> states[]; // varför static?
	static int currentState;                        // varför static?
	static int oldState;                            // varför static?
	UIMainMenu           menu;
	Window*              window = nullptr;
	UPtr<DirectX::Mouse> mouse;
   UPtr<Map>            map;
   UPtr<Voronoi>        voronoi;
	UPtr<GameObject>     parentTest;
	UPtr<GameObject>     childTest;
	UPtr<AIPlayer>       aiObject;
   Vec<GameObject>      markers;
   Vec<GameObject>      road_tiles;
   Vec<GameObject>      house_tiles;
   LightList            lightList;
   Vehicle              player;
	DynamicCamera        camera;
	F32                  deltaTime        = 0.0f;
	I64                  curTime          = 0;
	I64                  prevTime         = 0;
	I64                  countsPerSec     = 0;
	F32                  secPerCount      = 1.0f / countsPerSec;
	I32                  RadioButtonValue = 0;
	Arr<SpotLight, LightList::maxSize>::iterator playerLight;
   RoadNetwork *testNetwork = nullptr;

	void          generateMap( I32 seed );
   void          generateBuildings( RNG & );
   Opt<Vec<V2u>> find_valid_house_lot( RNG &, U16 cell_id, Voronoi const &district_map, Map &, Vec<District> const &lookup_tbl );

public:
	Game();
	~Game();
	void init(Window* window);
	void run();
};
