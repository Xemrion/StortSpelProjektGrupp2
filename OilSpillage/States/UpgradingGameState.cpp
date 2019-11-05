#include "UpgradingGameState.h"

UpgradingGameState::UpgradingGameState() : graphics(Game::getGraphics())
{
	this->menu = std::make_unique<UIUpgrading>();
	this->menu->init();
}

UpgradingGameState::~UpgradingGameState()
{
}

void UpgradingGameState::update(float deltaTime)
{
	this->graphics.clearScreen(Vector4(1,1,1,0));
	this->menu->update(deltaTime);
	this->graphics.presentScene();
}
