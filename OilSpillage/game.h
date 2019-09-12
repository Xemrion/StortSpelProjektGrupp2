#pragma once
#include "window.h"
#include "graphics.h"
#include"Keyboard.h"
#include"Mouse.h"
class Game {
	Window* window;

	std::unique_ptr<DirectX::Keyboard> keyboard;
	std::unique_ptr<DirectX::Mouse> mouse;
	static Graphics graphics;
	GameObject* testObject;
	GameObject* testObject2;
	void addQuad(int x);

	float deltaTime;
	__int64 curTime = 0;
	__int64 prevTime = 0;
	__int64 countsPerSec = 0;
	float secPerCount = 1.0f / countsPerSec;
public:
	void init(Window* window);
	void run();
	static Graphics& getGraphics()
	{
		return graphics;
	};
	
};
