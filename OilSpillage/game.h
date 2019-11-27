#pragma once

#include "window.h"
#include "graphics.h"
#include "States/GameState.h"
#include "physics.h"
#include"Vehicle.h"
//#define RELEASE_DEBUG

struct GameInfo
{
	int nrOfClearedStages = 0;
	int nrOfObjectives = 0;
	int nrOfSwarm = 0;
	int nrOfAttackers = 0;
	int nrOfCars = 0;
	int nrOfBosses = 0;
	int highScore = 0;
	float time = 0.0f;
};

class Game
{
private:
	static std::unique_ptr<Game> instance;
public:
	enum State
	{
		STATE_MENU,
		STATE_PLAYING,
		STATE_UPGRADING,
		STATE_HIGHSCORE
	};
	static void							start(Window* window);
	static void							quit();
	static GameState				   *getCurrentState();
	static int							getCurrentStateIndex();
	static void							setState(State);
	static Graphics					   &getGraphics() noexcept;
	static float						lerp(float v0, float v1, float t);
	static float						getDeltaTime();
	static float						getLocalScale();
	static int							getNrOfStagesDone();
	static GameInfo                    &getGameInfo() noexcept;
	static bool						    getDrivingMode();
	static void						    setDrivingMode(bool realistic);
private:
	void								createCurrentState();
	void								run();
	bool								running;
	Graphics							graphics;
	GameInfo							gameInfo;
	std::unique_ptr<GameState>			state;
	int									currentState;
	int									oldState;                         
	Window                             *window       = nullptr;

	float								localScale = 1.0f;
	int									nrOfStagesDone = 0;
	float								deltaTime    = 0.0f;
	long long							curTime      = 0;
	long long							prevTime     = 0;
	long long							countsPerSec = 0;
	float								secPerCount  = 0.0f;
	Vehicle*							thePlayer = nullptr;
	bool								drivingMode = false;
public:
	Game();
	~Game();
};
