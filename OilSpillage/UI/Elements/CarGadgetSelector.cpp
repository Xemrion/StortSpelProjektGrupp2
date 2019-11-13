#include "CarGadgetSelector.h"
#include "../UserInterface.h"
#include "../../game.h"
#include <cassert>
#include "../../States/UpgradingGameState.h"

Vector2 CarGadgetSelector::size = Vector2(180, 300);

CarGadgetSelector::CarGadgetSelector(Vector2 position) : Element(position)
{
	Game::getGraphics().loadTexture("UI/carTopdown");
	this->textureBG = Game::getGraphics().getTexturePointer("UI/carTopdown");
	assert(textureBG && "Texture failed to load!");

	this->slots[Slots::FRONT] = std::make_unique<ItemSlot>(position + Vector2(CarGadgetSelector::size.x / 2 - ItemSlot::size.x / 2, -ItemSlot::size.y - 10.0f), false);
	this->slots[Slots::MOUNTED] = std::make_unique<ItemSlot>(position + Vector2(CarGadgetSelector::size.x / 2 - ItemSlot::size.x / 2, CarGadgetSelector::size.y / 2 - ItemSlot::size.y / 2), false);
	this->slots[Slots::BACK] = std::make_unique<ItemSlot>(position + Vector2(CarGadgetSelector::size.x / 2 - ItemSlot::size.x / 2, CarGadgetSelector::size.y + 10.0f), false);
	this->slots[Slots::LEFT] = std::make_unique<ItemSlot>(position + Vector2(-ItemSlot::size.x - 10.0f, CarGadgetSelector::size.y / 2 - ItemSlot::size.y / 2), false);
	this->slots[Slots::RIGHT] = std::make_unique<ItemSlot>(position + Vector2(CarGadgetSelector::size.x + 10.0f, CarGadgetSelector::size.y / 2 - ItemSlot::size.y / 2), false);

	this->slots[Slots::FRONT]->setNeighbours(this->slots[Slots::LEFT].get(), this->slots[Slots::RIGHT].get(), this->slots[Slots::BACK].get(), this->slots[Slots::MOUNTED].get());
	this->slots[Slots::MOUNTED]->setNeighbours(this->slots[Slots::LEFT].get(), this->slots[Slots::RIGHT].get(), this->slots[Slots::FRONT].get(), this->slots[Slots::BACK].get());
	this->slots[Slots::BACK]->setNeighbours(this->slots[Slots::LEFT].get(), this->slots[Slots::RIGHT].get(), this->slots[Slots::MOUNTED].get(), this->slots[Slots::FRONT].get());
	this->slots[Slots::LEFT]->setNeighbours(this->slots[Slots::RIGHT].get(), this->slots[Slots::MOUNTED].get(), this->slots[Slots::FRONT].get(), this->slots[Slots::BACK].get());
	this->slots[Slots::RIGHT]->setNeighbours(this->slots[Slots::MOUNTED].get(), this->slots[Slots::LEFT].get(), this->slots[Slots::FRONT].get(), this->slots[Slots::BACK].get());
	this->selected = this->slots[Slots::FRONT].get();

	this->used = std::make_unique<Container::Slot*[]>(Slots::SIZEOF);
}

CarGadgetSelector::~CarGadgetSelector()
{
}

void CarGadgetSelector::draw(bool selected)
{
	UserInterface::getSpriteBatch()->Draw(this->textureBG->getShaderResView(), this->position);

	for (int i = 0; i < Slots::SIZEOF; i++)
	{
		this->slots[i]->draw(selected && this->selected == this->slots[i].get());
	}
}

void CarGadgetSelector::update(bool selected, float deltaTime)
{
	if (selected)
	{
		this->selected = UserInterface::changeSelected(this->selected);
	}
}

void CarGadgetSelector::setSlotOfSelected(Container::Slot* slot)
{
	for (int i = 0; i < Slots::SIZEOF; i++)
	{
		if (this->slots[i]->getSlot() == slot)
		{
			this->slots[i]->setSlot(nullptr);
			this->used[i] = nullptr;
			static_cast<UpgradingGameState*>(Game::getCurrentState())->getVehicle()->setSpecSlot(static_cast<Slots>(i), nullptr);
		}
	}

	if (this->selected == this->slots[Slots::LEFT].get() || this->selected == this->slots[Slots::RIGHT].get())
	{
		this->slots[Slots::LEFT]->setSlot(slot);
		this->slots[Slots::RIGHT]->setSlot(slot);
		this->used[Slots::LEFT] = slot;
		this->used[Slots::RIGHT] = slot;

		Item* newItem = slot->item->clone();
		static_cast<UpgradingGameState*>(Game::getCurrentState())->getVehicle()->setSpecSlot(Slots::RIGHT, newItem);

		newItem = slot->item->clone();
		static_cast<UpgradingGameState*>(Game::getCurrentState())->getVehicle()->setSpecSlot(Slots::LEFT, newItem);
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
				Item* newItem = slot->item->clone();
				static_cast<UpgradingGameState*>(Game::getCurrentState())->getVehicle()->setSpecSlot(static_cast<Slots>(i), newItem);
				break;
			}
		}
	}
}

Container::Slot** CarGadgetSelector::getUsed()
{
	return this->used.get();
}