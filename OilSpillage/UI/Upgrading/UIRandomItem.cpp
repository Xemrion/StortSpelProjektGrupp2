#include "UIRandomItem.h"
#include "../../Inventory/Container.h"
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
				Container::playerInventory->addItem(slots[i]->getSlot()->getItem()->clone());

				for (int j = 0; j < UIRandomItem::slotCount; j++)
				{
					slots[j]->setSlot(nullptr); //Remove from draw
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
	this->promptBar->draw(false);

	for (int i = 0; i < UIRandomItem::slotCount; i++)
	{
		slots[i]->draw(slots[i].get() == this->selected);
	}

	UserInterface::getSpriteBatch()->Draw(this->textureIndicator->getShaderResView(), this->selected->getPosition());
	Vector2 textSize = UserInterface::getFontArial()->MeasureString("Select Reward") * 0.5f;
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Select Reward", Vector2((SCREEN_WIDTH / 2) - (textSize.x / 2), (SCREEN_HEIGHT / 2) - 110.0f), Colors::White, 0.0f, Vector2::Zero, 0.5f);
	UserInterface::getSpriteBatch()->End();
}

UIRandomItem::UIRandomItem() : textureIndicator(nullptr), selected(nullptr), slots{}
{
}

UIRandomItem::~UIRandomItem()
{
}

void UIRandomItem::init()
{
	Game::getGraphics().loadTexture("UI/itemSelectorIndicator");
	this->textureIndicator = Game::getGraphics().getTexturePointer("UI/itemSelectorIndicator");
	assert(textureIndicator && "Texture failed to load!");

	for (int i = 0; i < UIRandomItem::slotCount; i++)
	{
		slots[i] = std::make_unique<ItemSlot>(Vector2(SCREEN_WIDTH / 2 - (UIRandomItem::slotCount / 2.0f) * (ItemSlot::size.x + 20) + i * 20.0f, SCREEN_HEIGHT / 2) + Vector2(i, -0.5f) * ItemSlot::size);
		this->container[i].setItem(Item::getRandom());
	}

	for (int i = 0; i < UIRandomItem::slotCount; i++)
	{
		slots[i]->setNeighbours(slots[(i - 1 + UIRandomItem::slotCount) % UIRandomItem::slotCount].get(), slots[(i + 1) % UIRandomItem::slotCount].get(), nullptr, nullptr);
		slots[i]->setSlot(&this->container[i]);
	}

	this->selected = slots[0].get();

	Prompt prompts[] = {
		{ Keys::L_PRESS, "Move", Color(Colors::White) },
		{ Keys::CONFIRM, "Select Item", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 2);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}
