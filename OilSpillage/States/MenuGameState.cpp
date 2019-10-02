#include "MenuGameState.h"
#include "../UI/UIMainMenu.h"
#include "../UI/UIOptions.h"

MenuGameState::MenuGameState() : graphics(Game::getGraphics())
{
	this->currentMenu = 0;
}

MenuGameState::~MenuGameState()
{
}

void MenuGameState::init()
{
	this->menues[MENU_MAIN] = std::make_unique<UIMainMenu>();
	this->menues[MENU_MAIN]->init();
	this->menues[MENU_OPTIONS] = std::make_unique<UIOptions>();
	this->menues[MENU_OPTIONS]->init();

	this->currentMenu = MENU_MAIN;
}

void MenuGameState::cleanUp()
{
	for (int i = 0; i < MENUCOUNT; i++)
	{
		this->menues[i].reset();
	}
}

void MenuGameState::update(float deltaTime)
{
	this->graphics.clearScreen();
	this->menues[this->currentMenu]->update(deltaTime);
	this->graphics.presentScene();
}

void MenuGameState::setCurrentMenu(Menu menu)
{
	this->currentMenu = static_cast<int>(menu);
}
