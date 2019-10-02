#include "MenuGameState.h"

MenuGameState::MenuGameState() : graphics(Game::getGraphics())
{
}

MenuGameState::~MenuGameState()
{
}

void MenuGameState::init()
{
	this->mainMenu = std::make_unique<UIMainMenu>();
	this->mainMenu->init();
}

void MenuGameState::cleanUp()
{
	this->mainMenu.reset();
}

void MenuGameState::update(float deltaTime)
{
	this->mainMenu->update(deltaTime);
	this->graphics.presentScene();
}
