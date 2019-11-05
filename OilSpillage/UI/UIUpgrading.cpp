#include "UIUpgrading.h"
#include "../Input.h"
#include "../game.h"

void UIUpgrading::updateUI(float deltaTime)
{
	if (Input::checkButton(Keys::L_LEFT, States::PRESSED))
	{
		this->itemSelector->changeSelectedIndex(false);
	}
	else if(Input::checkButton(Keys::L_RIGHT, States::PRESSED))
	{
		this->itemSelector->changeSelectedIndex(true);
	}
	else if(Input::checkButton(Keys::L_UP, States::PRESSED))
	{
		this->itemSelector->changeSelectedType(false);
	}
	else if(Input::checkButton(Keys::L_DOWN, States::PRESSED))
	{
		this->itemSelector->changeSelectedType(true);
	}
}

void UIUpgrading::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->itemSelector->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIUpgrading::UIUpgrading()
{
}

UIUpgrading::~UIUpgrading()
{
}

void UIUpgrading::init()
{
	this->itemSelector = std::make_unique<ItemSelector>(Vector2(0, 0));
}
