#include "MenuGameState.h"

MenuGameState::MenuGameState()
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
	this->mainMenu.release();
}

void MenuGameState::update(float deltaTime)
{
	this->mainMenu->update(deltaTime);
}
