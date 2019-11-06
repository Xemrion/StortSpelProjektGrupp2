#include "UpgradingGameState.h"
#include "../Input.h"

UpgradingGameState::UpgradingGameState() : graphics(Game::getGraphics())
{
	this->menu = std::make_unique<UIUpgrading>();
	this->menu->init();

	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());
	Inventory::instance->addItem(Item::getRandom());

	Vector3 test = this->graphics.screenToWorldSpaceUI(Input::getWindowSize() * 0.5f);

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
