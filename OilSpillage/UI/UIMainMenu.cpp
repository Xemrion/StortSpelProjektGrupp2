#include "UIMainMenu.h"

void UIMainMenu::updateUI(float deltaTime)
{

}

void UIMainMenu::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->button->draw();
	UserInterface::getSpriteBatch()->End();
}

UIMainMenu::UIMainMenu()
{
}

UIMainMenu::~UIMainMenu()
{
}

void UIMainMenu::initUI()
{
	this->button = std::make_unique<Button>(Text("TEST OF BUTTON", Color(Colors::Red), Center), Vector2(0, 0));
}
