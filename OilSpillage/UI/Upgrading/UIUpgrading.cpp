#include "UIUpgrading.h"
#include "../../Input.h"
#include "../../game.h"

void UIUpgrading::updateUI(float deltaTime)
{
	if (Input::checkButton(Keys::L_LEFT, States::PRESSED))
	{
		if (this->vehicleSlots->getSlot(Slots::FRONT) != nullptr)
			Game::getGraphics().removeFromDraw(this->vehicleSlots->getSlot(Slots::FRONT)->getObject());
		this->itemSelector->changeSelectedIndex(false);
		Item* newItem = new Item(*this->itemSelector->getSelectedItem());
		if (newItem->getObject() != nullptr)
			Game::getGraphics().addToDraw(newItem->getObject());
		this->vehicleSlots->setSlot(Slots::FRONT, newItem);
	}
	else if (Input::checkButton(Keys::L_RIGHT, States::PRESSED))
	{
		if(this->vehicleSlots->getSlot(Slots::FRONT)!=nullptr)
			Game::getGraphics().removeFromDraw(this->vehicleSlots->getSlot(Slots::FRONT)->getObject());

		this->itemSelector->changeSelectedIndex(true);
		Item* newItem = new Item(*this->itemSelector->getSelectedItem());
		if(newItem->getObject()!=nullptr)
			Game::getGraphics().addToDraw(newItem->getObject());
		this->vehicleSlots->setSlot(Slots::FRONT, newItem);
	}
	else if (Input::checkButton(Keys::L_UP, States::PRESSED))
	{
		if (this->vehicleSlots->getSlot(Slots::FRONT) != nullptr)
			Game::getGraphics().removeFromDraw(this->vehicleSlots->getSlot(Slots::FRONT)->getObject());
		this->itemSelector->changeSelectedType(false);
		Item* newItem = new Item(*this->itemSelector->getSelectedItem());
		if (newItem->getObject() != nullptr)
			Game::getGraphics().addToDraw(newItem->getObject());
		this->vehicleSlots->setSlot(Slots::FRONT, newItem);
	}
	else if (Input::checkButton(Keys::L_DOWN, States::PRESSED))
	{
		if (this->vehicleSlots->getSlot(Slots::FRONT) != nullptr)
			Game::getGraphics().removeFromDraw(this->vehicleSlots->getSlot(Slots::FRONT)->getObject());
		this->itemSelector->changeSelectedType(true);
		Item* newItem = new Item(*this->itemSelector->getSelectedItem());
		if (newItem->getObject() != nullptr)
			Game::getGraphics().addToDraw(newItem->getObject());
		this->vehicleSlots->setSlot(Slots::FRONT, newItem);
	}

	this->itemSelector->update(deltaTime);
}

void UIUpgrading::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->itemSelector->draw(false);

	const char* type = "";
	switch (this->itemSelector->getSelectedType())
	{
	case WEAPON:
		type = "Weapons";
		break;
	case GADGET:
		type = "Gadgets";
		break;
	case CHASSI:
		type = "Chassis";
		break;
	case WHEEL:
		type = "Wheels";
		break;
	}

	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), type, Vector2(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2), Colors::White, 0, Vector2(0, 0), 0.5f);
	UserInterface::getSpriteBatch()->End();
}

UIUpgrading::UIUpgrading()
{
}

UIUpgrading::~UIUpgrading()
{
}

VehicleSlots* UIUpgrading::getVehicleSlots()
{
	return this->vehicleSlots.get();
}

void UIUpgrading::init()
{
	this->itemSelector = std::make_unique<ItemSelector>(Vector2(0, 0));
	this->vehicleSlots = std::make_unique<VehicleSlots>();

}
