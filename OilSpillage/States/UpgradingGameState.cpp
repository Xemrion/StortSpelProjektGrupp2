#include "UpgradingGameState.h"
#include "../Input.h"

UpgradingGameState::UpgradingGameState() : graphics(Game::getGraphics())
{
	this->menu = std::make_unique<UIUpgrading>();
	this->menu->init();

	for (int i = 0; i < 20; i++)
	{
		Inventory::instance->addItem(Item::getRandom());
	}
	
	graphics.setUISun(Vector3(0, 0, 1), Vector4(1, 1, 1, 1));
}

UpgradingGameState::~UpgradingGameState()
{
}

void UpgradingGameState::update(float deltaTime)
{
	this->graphics.clearScreen(Vector4(1,1,1,0));
	this->menu->update(deltaTime);
	this->graphics.renderUI(deltaTime);
	this->graphics.presentScene();
}
