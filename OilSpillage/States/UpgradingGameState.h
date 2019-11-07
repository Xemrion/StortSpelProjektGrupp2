#ifndef UPGRADING_GAME_STATE_H
#define UPGRADING_GAME_STATE_H

#include"../Vehicle.h"
#include "GameState.h"
#include "../game.h"
#include "../UI/Upgrading/UIUpgrading.h"

class UpgradingGameState : public GameState
{
private:
	Graphics& graphics;
	std::unique_ptr<UIUpgrading> menu;
	DynamicCamera* camera;
	Vehicle* theVehicle;
	std::unique_ptr<LightList>      lightList;
	float timer;
	std::unique_ptr<Physics>		physics;//for vehicle


public:
	UpgradingGameState();
	virtual ~UpgradingGameState();

	void update(float deltaTime);
};

#endif // !UPGRADING_GAME_STATE_H
