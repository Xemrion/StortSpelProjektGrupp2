#include "UIControls.h"
#include "../../Input.h"
#include "../../game.h"
#include "../../States/MenuGameState.h"
#include <cassert>

void UIControls::updateUI(float deltaTime)
{
	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		Game::setState(Game::STATE_UPGRADING);
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
	Game::getGraphics().unloadTexture("UI/controls", false);
}

void UIControls::init()
{
	std::string path = std::string(TEXTURE_ROOT_DIR) + std::string("UI/controls") + std::string(".dds");
	Game::getGraphics().loadTexture(path, true);
	this->textureControls = Game::getGraphics().getTexturePointer("UI/controls", false);
	assert(textureControls && "Texture not loaded correctly!");

	Prompt prompts[] = {
		{ Keys::CONFIRM, "Start Game", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 1);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}
