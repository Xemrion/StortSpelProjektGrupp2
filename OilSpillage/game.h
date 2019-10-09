#pragma once

#include "window.h"
#include "graphics.h"
#include "States/GameState.h"

enum State
{
	STATE_MENU,
	STATE_PLAYING,
	STATECOUNT
};

class Game
{
public:
	static GameState       *getCurrentState();
	static void             setState(State);
	inline static Graphics &getGraphics() noexcept { return graphics; };
private:
	static Graphics                    graphics;
	static std::unique_ptr<GameState>  states[];  
	static int                         currentState;                     
	static int                         oldState;                         
	Window                            *window       = nullptr;
	float                              deltaTime    = 0.0f;
	long long                          curTime      = 0;
	long long                          prevTime     = 0;
	long long                          countsPerSec = 0;
	float                              secPerCount  = 1.0f / countsPerSec;
public:
	Game();
	~Game();
	void init(Window*);
	void run();
};
