#pragma once

#include "window.h"
#include "graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "ImGui/imgui.h"
//#include"ImGui/imgui_impl_win32.h"
//#include"ImGui/imgui_impl_dx11.h"
#include "PG/District.hpp"
#include "PG/Voronoi.hpp"
#include "PG/Map.hpp"
#include "PG/defs.hpp"
#include "vehicle.h"
#include "AI/Actor.h"
#include "Road Network/RoadNet.h"
#include "DynamicCamera.h"
#include <array>
#include "UI/UIMainMenu.h"

class Game {
	UIMainMenu           menu;
	Window*              window = nullptr;
	UPtr<DirectX::Mouse> mouse;
	static Graphics      graphics;
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
   
   RoadNetwork* testNetwork = nullptr;
   // AStar aStar; TODO
	// TestAI AI;

	void          generateMap( I32 seed );
   void          generateBuildings( RNG & );
   Opt<Vec<V2u>> find_valid_house_lot( RNG &, U16 cell_id, Voronoi const &district_map, Map &, Vec<District> const &lookup_tbl );
	void          initiateAStar();

public:
	Game();
	~Game();
	void                    init(Window*);
	void                    run();
	inline static Graphics& getGraphics() noexcept { return graphics; };
};
