#include "UIControls.h"
#include "../../Input.h"
#include "../../game.h"
#include "../../States/MenuGameState.h"
#include <cassert>

void UIControls::updateUI(float deltaTime)
{
	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		Game::setState(Game::STATE_UPGRADING); //Game::setState(Game::STATE_PLAYING);
	}
	else if (Input::checkButton(Keys::CANCEL, States::PRESSED))
	{
		static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_MAIN);
	}
}

void UIControls::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	UserInterface::getSpriteBatch()->Draw(this->textureControls->getShaderResView(), Vector2());
	this->promptBar->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIControls::UIControls() : textureControls(nullptr)
{
}

UIControls::~UIControls()
{
}

void UIControls::init()
{
	Game::getGraphics().loadTexture("UI/controls");
	this->textureControls = Game::getGraphics().getTexturePointer("UI/controls");
	assert(textureControls && "Texture not loaded correctly!");

	Prompt prompts[] = {
		{ Keys::CONFIRM, "Start Game", Color(Colors::White) },
		{ Keys::CANCEL, "Cancel/Back", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 2);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}
