#pragma once
#include "window.h"
#include "graphics.h"
#include "Road Network/RoadNet.h"

class Game {
	Window* window;
	Graphics graphics;
	GameObject* testObject;
	GameObject* testObject2;
	RoadNetwork* testNetwork;
	void addQuad(int x);
public:
	void init(Window* window);
	void run();
	
};
