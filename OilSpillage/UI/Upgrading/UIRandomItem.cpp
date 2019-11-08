#include "UIRandomItem.h"
#include "../../Inventory/Inventory.h"
#include "../../game.h"
#include "../../States/UpgradingGameState.h"
#include "../../Input.h"

void UIRandomItem::updateUI(float deltaTime)
{
	this->selected = UserInterface::changeSelected(this->selected);

	for (int i = 0; i < UIRandomItem::slotCount; i++)
	{
		slots[i]->update(slots[i].get() == this->selected, deltaTime);
	}

	if (Input::checkButton(Keys::CONFIRM, States::HELD))
	{
		for (int i = 0; i < UIRandomItem::slotCount; i++)
		{
			if (slots[i].get() == this->selected)
			{
				Inventory::instance->addItem(slots[i]->getItem());

				for (int j = 0; j < UIRandomItem::slotCount; j++)
				{
					slots[j]->setItem(nullptr);
				}

				static_cast<UpgradingGameState*>(Game::getCurrentState())->setCurrentMenu(UpgradingGameState::MENU_UPGRADING);
				break;
			}
		}
	}

	
}

void UIRandomItem::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());

	for (int i = 0; i < UIRandomItem::slotCount; i++)
	{
		slots[i]->draw(slots[i].get() == this->selected);
	}
	
	UserInterface::getSpriteBatch()->End();
}

UIRandomItem::UIRandomItem() : selected(nullptr), slots{}
{
}

UIRandomItem::~UIRandomItem()
{
}

void UIRandomItem::init()
{
	for (int i = 0; i < UIRandomItem::slotCount; i++)
	{
		slots[i] = std::make_unique<ItemSlot>(Vector2(SCREEN_WIDTH / 2 - (UIRandomItem::slotCount / 2.0f) * (ItemSlot::size.x + 20) + i * 20.0f, SCREEN_HEIGHT / 2) + Vector2(i, -0.5f) * ItemSlot::size);
	}

	for (int i = 0; i < UIRandomItem::slotCount; i++)
	{
		slots[i]->setNeighbours(slots[(i - 1 + UIRandomItem::slotCount) % UIRandomItem::slotCount].get(), slots[(i + 1) % UIRandomItem::slotCount].get(), nullptr, nullptr);
		slots[i]->setItem(Item::getRandom());
	}

	this->selected = slots[0].get();
}
