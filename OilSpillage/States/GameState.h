#pragma once

class GameState
{
public:
	GameState() {};
	virtual ~GameState() {};

	virtual void init() = 0;
	virtual void cleanUp() = 0;
	virtual void update(float deltaTime) = 0;
};
