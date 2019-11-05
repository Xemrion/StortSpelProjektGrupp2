#include "UIControls.h"
#include "../../Input.h"
#include "../../game.h"
#include "../../States/MenuGameState.h"
#include <cassert>

void UIControls::updateUI(float deltaTime)
{
	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		Game::setState(Game::STATE_PLAYING);
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
	Vector2 size = UserInterface::getFontArial()->MeasureString("Press confirm to continue");
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Press confirm to continue", Vector2(SCREEN_WIDTH / 2, 600), Colors::Yellow, 0, size / 2);
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
}
