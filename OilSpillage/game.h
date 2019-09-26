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
#include <memory>
#include "PG/defs.hpp"

#include "DynamicCamera.h"

#include <array>

class Game {
	Window* window = nullptr;

	std::unique_ptr<DirectX::Mouse> mouse;
	static Graphics graphics;
	GameObject* testObject = nullptr;
	GameObject* testObject2 = nullptr;
	Vec<GameObject> tiles;
	
	//GameObject* AiTestObject = nullptr;
	GameObject* testObject3 = nullptr;
	GameObject* parentTest = nullptr;
	GameObject* childTest = nullptr;
	AIPlayer * aiObject = nullptr;
	LightList lightList;
	void addQuad(int x);

	float deltaTime = 0.0f;
	__int64 curTime = 0;
	__int64 prevTime = 0;
	__int64 countsPerSec = 0;
	float secPerCount = 1.0f / countsPerSec;
	
	Vehicle player;
	DynamicCamera camera;
	//TestAI AI;

	void generateMap();

	int RadioButtonValue;
	std::array<SpotLight, LightList::maxSize>::iterator playerLight;

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
