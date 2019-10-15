#pragma once

#include "window.h"
#include "graphics.h"
#include "States/GameState.h"
#include "physics.h"

class Game
{
private:
	static std::unique_ptr<Game> instance;
public:
	enum State
	{
		STATE_MENU,
		STATE_PLAYING
	};
	static void							start(Window* window);
	static GameState				   *getCurrentState();
	static void							setState(State);
	static Graphics					   &getGraphics() noexcept;

private:
	void								createCurrentState();
	void								run();
	Graphics							graphics;
	std::unique_ptr<GameState>			state;
	int									currentState;
	int									oldState;                         
	Window                             *window       = nullptr;
	float								deltaTime    = 0.0f;
	long long							curTime      = 0;
	long long							prevTime     = 0;
	long long							countsPerSec = 0;
	float								secPerCount  = 0.0f;
public:
	Game();
	~Game();
};
