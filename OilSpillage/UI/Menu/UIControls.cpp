#include "UIControls.h"
#include "../../Input.h"
#include "../../game.h"
#include "../../States/PlayingGameState.h"
#include <cassert>

void UIControls::updateUI(float deltaTime)
{
	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		PlayingGameState* state = dynamic_cast<PlayingGameState*>(Game::getCurrentState());

		if (state)
		{
			state->setCurrentMenu(PlayingGameState::MENU_PAUSED);
		}
		else
		{
			Game::setState(Game::STATE_UPGRADING);
		}
	}
}

void UIControls::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	UserInterface::getSpriteBatch()->Draw(this->textureControls->getShaderResView(), Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), nullptr, Colors::White, 0.0f, this->textureControls->getCenter());
	this->promptBar->draw(false);
	this->promptBar2->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIControls::UIControls() : textureControls(nullptr)
{
}

UIControls::~UIControls()
{
	Game::getGraphics().unloadTexture("UI/controls");
}

void UIControls::init()
{
	Game::getGraphics().loadTexture("UI/controls");
	this->textureControls = Game::getGraphics().getTexturePointer("UI/controls");
	assert(textureControls && "Texture not loaded correctly!");

	Prompt prompts[] = {
		{ Keys::CONFIRM, "Continue", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 1);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));

	Prompt prompts2[] = {
		{ Keys::L_PRESS, "Steering", Color(Colors::White) },
		{ Keys::R_PRESS, "Mounted Weapon", Color(Colors::White) },
		{ Keys::R_SHOULDER, "Side Weapons", Color(Colors::White) },
		{ Keys::L_SHOULDER, "Gadget", Color(Colors::White) },
		{ Keys::MENU, "Pause", Color(Colors::White) },
		{ Keys::R_TRIGGER, "Accelerate", Color(Colors::White) },
		{ Keys::L_TRIGGER, "Brake", Color(Colors::White) },
	};

	this->promptBar2 = std::make_unique<ButtonPromptBar>(prompts2, Game::getDrivingMode() ? 7 : 5);
	this->promptBar2->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar2->getSize().x / 2, SCREEN_HEIGHT - this->promptBar2->getSize().y - 16.0f - this->promptBar->getSize().y - 8.0f));
}
