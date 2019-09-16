#pragma once
#include "window.h"
#include "graphics.h"
#include"Keyboard.h"
#include"Mouse.h"
#include"vehicle.h"
#include"TestAI.h"

class Game {
	Window* window = nullptr;

	std::unique_ptr<DirectX::Keyboard> keyboard;
	std::unique_ptr<DirectX::Mouse> mouse;
	static Graphics graphics;
	GameObject* testObject = nullptr;
	GameObject* testObject2 = nullptr;
	GameObject* AiTestObject = nullptr;
	void addQuad(int x);

	float deltaTime = 0.0f;
	__int64 curTime = 0;
	__int64 prevTime = 0;
	__int64 countsPerSec = 0;
	float secPerCount = 1.0f / countsPerSec;
	
	Vehicle player;
	TestAI AI;

public:
	void init(Window* window);
	void run();
	static Graphics& getGraphics()
	{
		return graphics;
	};
	
};
