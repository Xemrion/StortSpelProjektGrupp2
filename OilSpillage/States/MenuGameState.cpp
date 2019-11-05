#include "MenuGameState.h"
#include "../UI/Menu/UIMainMenu.h"
#include "../UI/Menu/UIOptions.h"
#include "../UI/Menu/UICredits.h"
#include "../UI/Menu/UIControls.h"
#include <cassert>

MenuGameState::MenuGameState() : graphics(Game::getGraphics()), currentMenu(MENU_MAIN)
{
	this->menues[MENU_MAIN] = std::make_unique<UIMainMenu>();
	this->menues[MENU_MAIN]->init();
	this->menues[MENU_OPTIONS] = std::make_unique<UIOptions>();
	this->menues[MENU_OPTIONS]->init();
	this->menues[MENU_CREDITS] = std::make_unique<UICredits>();
	this->menues[MENU_CREDITS]->init();
	this->menues[MENU_CONTROLS] = std::make_unique<UIControls>();
	this->menues[MENU_CONTROLS]->init();

	Game::getGraphics().loadTexture("UI/menuBG");
	this->textureBG = Game::getGraphics().getTexturePointer("UI/menuBG");
	assert(textureBG && "Could not load texture!");
}

MenuGameState::~MenuGameState() {}

void MenuGameState::update(float deltaTime)
{
	this->graphics.clearScreen(Vector4(0,0,0,0));

	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	UserInterface::getSpriteBatch()->Draw(this->textureBG->getShaderResView(), Vector2());
	UserInterface::getSpriteBatch()->End();

	this->menues[this->currentMenu]->update(deltaTime);
	this->graphics.presentScene();
}

void MenuGameState::setCurrentMenu(Menu menu)
{
	this->currentMenu = static_cast<int>(menu);
}
