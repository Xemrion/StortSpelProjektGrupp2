#pragma once
#include "window.h"
#include "graphics.h"

class Game {
	Window* window;
	Graphics graphics;
public:
	void init(Window* window);
	void run();
};
