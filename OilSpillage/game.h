#pragma once
#include "window.h"
#include "graphics.h"
#include"Keyboard.h"
#include"Mouse.h"
class Game {
	Window* window;

	std::unique_ptr<DirectX::Keyboard> keyboard;
	std::unique_ptr<DirectX::Mouse> mouse;
	Graphics graphics;
	GameObject* testObject;
	GameObject* testObject2;
	void addQuad(int x);
public:
	void init(Window* window);
	void run();
	
};
