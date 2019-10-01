#pragma once
#include "window.h"
#include "graphics.h"
#include"Keyboard.h"
#include"Mouse.h"
#include"ImGui/imgui.h"
//#include"ImGui/imgui_impl_win32.h"
//#include"ImGui/imgui_impl_dx11.h"
#include"vehicle.h"
#include "AI/Actor.h"
#include "Road Network/RoadNet.h"

#include "DynamicCamera.h"
#include "PG/Map.hpp"
#include "PG/defs.hpp"
#include <array>

class Game {
	Window* window = nullptr;

	std::unique_ptr<DirectX::Mouse> mouse;
	static Graphics graphics;
	GameObject* testObject  = nullptr;
	GameObject* testObject2 = nullptr;
	GameObject* testObject3 = nullptr;
	GameObject* parentTest = nullptr;
	GameObject* childTest = nullptr;
	AIPlayer* aiObject = nullptr;
	LightList lightList;
	// AStar aStar; TODO
	UPtr<Map> map;
	Vec<GameObject> tiles;
	//GameObject* AiTestObject = nullptr;
	AIPlayer * aiObject = nullptr;
	RoadNetwork* testNetwork = nullptr;

	void addQuad(int x);

	float deltaTime = 0.0f;
	__int64 curTime = 0;
	__int64 prevTime = 0;
	__int64 countsPerSec = 0;
	float secPerCount = 1.0f / countsPerSec;
	
	Vehicle player;
	DynamicCamera camera;
	//TestAI AI;

	int RadioButtonValue;
	std::array<SpotLight, LightList::maxSize>::iterator playerLight;

	void generateMap();
	void initiateAStar();

public:
	Game();
	~Game();
	void init(Window* window);
	void run();
	static Graphics& getGraphics()
	{
		return graphics;
	};
	
};
