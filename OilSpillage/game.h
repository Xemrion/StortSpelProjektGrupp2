#pragma once

#include "window.h"
#include "graphics.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "ImGui/imgui.h"
//#include"ImGui/imgui_impl_win32.h"
//#include"ImGui/imgui_impl_dx11.h"
#include "vehicle.h"
#include "AI/Actor.h"
#include "Road Network/RoadNet.h"

#include "DynamicCamera.h"
#include "PG/Map.hpp"
#include "PG/defs.hpp"
#include <array>

class Game {
	Window*              window = nullptr;
	UPtr<DirectX::Mouse> mouse;
	static Graphics      graphics;
   UPtr<Map>            map;
	UPtr<GameObject>     testObject;
	UPtr<GameObject>     testObject2;
	UPtr<GameObject>     testObject3;
	UPtr<GameObject>     parentTest;
	UPtr<GameObject>     childTest;
	UPtr<AIPlayer>       aiObject;
   Vec<GameObject>      markers;
   Vec<GameObject>      tiles;
   LightList            lightList;
   Vehicle              player;
	DynamicCamera        camera;
	F32                  deltaTime     = 0.0f;
	I64                  curTime       = 0;
	I64                  prevTime      = 0;
	I64                  countsPerSec  = 0;
	F32                  secPerCount   = 1.0f / countsPerSec;
	I32                  RadioButtonValue;
	Arr<SpotLight, LightList::maxSize>::iterator playerLight;
   // AStar aStar; TODO
	// TestAI AI;

   void addQuad(int x);

   RoadNetwork* testNetwork = nullptr;

	void generateMap();
	void initiateAStar();

public:
	Game();
	~Game();
	void init(Window*);
	void run();
	inline static Graphics& getGraphics() noexcept { return graphics; };
};
