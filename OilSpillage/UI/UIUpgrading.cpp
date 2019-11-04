#include "UIUpgrading.h"
#include "../Input.h"
#include "../game.h"

void UIUpgrading::updateUI(float deltaTime)
{
	//TODO
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
