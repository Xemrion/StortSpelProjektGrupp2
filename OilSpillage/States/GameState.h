#ifndef GAME_STATE_H
#define GAME_STATE_H

class GameState
{
public:
	GameState() {};
	virtual ~GameState() {};

	virtual void init() = 0;
	virtual void cleanUp() = 0;
	virtual void update(float deltaTime) = 0;
};

#endif // !GAME_STATE_H
