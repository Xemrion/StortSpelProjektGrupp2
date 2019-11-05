#include "UpgradingGameState.h"

UpgradingGameState::UpgradingGameState() : graphics(Game::getGraphics())
{
	this->menu = std::make_unique<UIUpgrading>();
	this->menu->init();

	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
}

UpgradingGameState::~UpgradingGameState()
{
}

void UpgradingGameState::update(float deltaTime)
{
	this->graphics.clearScreen(Vector4(1, 1, 1, 1));
	this->menu->update(deltaTime);
	this->graphics.presentScene();
}
