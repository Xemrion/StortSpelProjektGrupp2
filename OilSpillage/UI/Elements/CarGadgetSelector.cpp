#include "CarGadgetSelector.h"
#include "../UserInterface.h"
#include "../../game.h"
#include <cassert>
#include "../../States/UpgradingGameState.h"

Vector2 CarGadgetSelector::size = Vector2(325, 490);

CarGadgetSelector::CarGadgetSelector(Vector2 position) : Element(position), used{ nullptr }
{
	Game::getGraphics().loadTexture("UI/statBG");
	Game::getGraphics().loadTexture("UI/carTopdown");
	Game::getGraphics().loadTexture("UI/itemSelectorIndicator");
	Game::getGraphics().loadTexture("UI/itemSelectorIndicatorError");
	this->textureBG = Game::getGraphics().getTexturePointer("UI/statBG");
	this->textureCar = Game::getGraphics().getTexturePointer("UI/carTopdown");
	this->textureIndicator = Game::getGraphics().getTexturePointer("UI/itemSelectorIndicator");
	this->textureIndicatorError = Game::getGraphics().getTexturePointer("UI/itemSelectorIndicatorError");
	assert(textureBG && "Texture failed to load!");
	assert(textureCar && "Texture failed to load!");
	assert(textureIndicator && "Texture failed to load!");
	assert(textureIndicatorError && "Texture failed to load!");

	this->slots[Slots::FRONT] = std::make_unique<ItemSlot>(position + Vector2(CarGadgetSelector::size.x / 2 - ItemSlot::size.x / 2, 32.0f), false);
	this->slots[Slots::MOUNTED] = std::make_unique<ItemSlot>(position + Vector2(CarGadgetSelector::size.x / 2 - ItemSlot::size.x / 2, CarGadgetSelector::size.y / 2 - ItemSlot::size.y / 2), false);
	this->slots[Slots::BACK] = std::make_unique<ItemSlot>(position + Vector2(CarGadgetSelector::size.x / 2 - ItemSlot::size.x / 2, CarGadgetSelector::size.y - ItemSlot::size.y - 32.0f), false);
	this->slots[Slots::LEFT] = std::make_unique<ItemSlot>(position + Vector2(21.0f, CarGadgetSelector::size.y / 2 - ItemSlot::size.y / 2), false);
	this->slots[Slots::RIGHT] = std::make_unique<ItemSlot>(position + Vector2(CarGadgetSelector::size.x - ItemSlot::size.y - 21.0f, CarGadgetSelector::size.y / 2 - ItemSlot::size.y / 2), false);
	this->slots[Slots::WHEEL] = std::make_unique<ItemSlot>(Vector2(-1000), false);
	this->slots[Slots::CHASSI] = std::make_unique<ItemSlot>(Vector2(-1000), false);

	this->slots[Slots::FRONT]->setNeighbours(this->slots[Slots::LEFT].get(), this->slots[Slots::RIGHT].get(), this->slots[Slots::BACK].get(), this->slots[Slots::MOUNTED].get());
	this->slots[Slots::MOUNTED]->setNeighbours(this->slots[Slots::LEFT].get(), this->slots[Slots::RIGHT].get(), this->slots[Slots::FRONT].get(), this->slots[Slots::BACK].get());
	this->slots[Slots::BACK]->setNeighbours(this->slots[Slots::LEFT].get(), this->slots[Slots::RIGHT].get(), this->slots[Slots::MOUNTED].get(), this->slots[Slots::FRONT].get());
	this->slots[Slots::LEFT]->setNeighbours(this->slots[Slots::RIGHT].get(), this->slots[Slots::MOUNTED].get(), this->slots[Slots::FRONT].get(), this->slots[Slots::BACK].get());
	this->slots[Slots::RIGHT]->setNeighbours(this->slots[Slots::MOUNTED].get(), this->slots[Slots::LEFT].get(), this->slots[Slots::FRONT].get(), this->slots[Slots::BACK].get());
	this->selected = this->slots[Slots::FRONT].get();
}

CarGadgetSelector::~CarGadgetSelector()
{

}

void CarGadgetSelector::draw(bool selected)
{
	UserInterface::getSpriteBatch()->Draw(this->textureBG->getShaderResView(), this->position);
	UserInterface::getSpriteBatch()->Draw(this->textureCar->getShaderResView(), this->position + (CarGadgetSelector::size * 0.5f - this->textureCar->getCenter()));

	for (int i = 0; i < 5/*Slots::SIZEOF*/; i++)
	{
		this->slots[i]->draw(slots[i].get() == this->selected);
	}

	if (selected)
	{
		if (this->selected == this->slots[Slots::BACK].get())
		{
			UserInterface::getSpriteBatch()->Draw(this->textureIndicatorError->getShaderResView(), this->selected->getPosition());
		}
		else
		{
			UserInterface::getSpriteBatch()->Draw(this->textureIndicator->getShaderResView(), this->selected->getPosition());
		}
	}
}

void CarGadgetSelector::update(bool selected, float deltaTime)
{
	if (selected)
	{
		this->selected = UserInterface::changeSelected(this->selected);
	}
}

void CarGadgetSelector::init()
{
	VehicleSlots* vehicleSlots = static_cast<UpgradingGameState*>(Game::getCurrentState())->getVehicle()->getSlots();

	for (int i = 0; i < Slots::SIZEOF; i++)
	{
		this->used[static_cast<Slots>(i)] = vehicleSlots->getInventorySlot(static_cast<Slots>(i));
		this->slots[i]->setSlot(vehicleSlots->getInventorySlot(static_cast<Slots>(i)));
	}
}

bool CarGadgetSelector::isBackSlot()
{
	return this->selected == this->slots[Slots::BACK].get();
}

void CarGadgetSelector::setSlot(Slots slots, Container::Slot* slot)
{
	auto temp = this->selected;
	this->selected = this->slots[slots].get();
	this->setSlotOfSelected(slot);
	this->selected = temp;
}

void CarGadgetSelector::setSlotOfSelected(Container::Slot* slot)
{
	for (int i = 0; i < Slots::SIZEOF; i++)
	{
		if (this->slots[i]->getSlot() == slot)
		{
			this->slots[i]->setSlot(nullptr);
			this->used[i] = nullptr;
			static_cast<UpgradingGameState*>(Game::getCurrentState())->getVehicle()->setSpecSlot(static_cast<Slots>(i), nullptr, nullptr);
		}
	}

	if (this->selected == this->slots[Slots::LEFT].get() || this->selected == this->slots[Slots::RIGHT].get())
	{
		this->slots[Slots::LEFT]->setSlot(slot);
		this->slots[Slots::RIGHT]->setSlot(slot);
		this->used[Slots::LEFT] = slot;
		this->used[Slots::RIGHT] = slot;

		static_cast<UpgradingGameState*>(Game::getCurrentState())->getVehicle()->setSpecSlot(Slots::RIGHT, slot->getItem()->clone(), slot);
		static_cast<UpgradingGameState*>(Game::getCurrentState())->getVehicle()->setSpecSlot(Slots::LEFT, slot->getItem()->clone(), slot);
	}
	else
	{
		ItemSlot* itemSlot = static_cast<ItemSlot*>(this->selected);
		itemSlot->setSlot(slot);

		for (int i = 0; i < Slots::SIZEOF; i++)
		{
			if (this->slots[i].get() == this->selected)
			{
				this->used[i] = slot;
				static_cast<UpgradingGameState*>(Game::getCurrentState())->getVehicle()->setSpecSlot(static_cast<Slots>(i), slot->getItem()->clone(), slot);
				break;
			}
		}
	}
}

void CarGadgetSelector::removeSlot(Container::Slot* slot)
{
	for (int i = 0; i < Slots::SIZEOF; i++)
	{
		if (this->slots[i]->getSlot() == slot)
		{
			this->slots[i]->setSlot(nullptr);
			this->used[i] = nullptr;
			static_cast<UpgradingGameState*>(Game::getCurrentState())->getVehicle()->setSpecSlot(static_cast<Slots>(i), nullptr, nullptr);
		}
	}
}

Container::Slot** CarGadgetSelector::getUsed()
{
	return this->used;
}

void CarGadgetSelector::unloadTextures()
{
	Game::getGraphics().unloadTexture("UI/statBG");
	Game::getGraphics().unloadTexture("UI/carTopdown");
	Game::getGraphics().unloadTexture("UI/itemSelectorIndicator");
}
