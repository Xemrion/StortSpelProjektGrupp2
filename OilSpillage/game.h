#pragma once
#include "window.h"
#include "graphics.h"

class Game {
	Window* window;
	Graphics graphics;
	GameObject* testObject;
	void addQuad(int x);
public:
	void init(Window* window);
	void run();
	
};
