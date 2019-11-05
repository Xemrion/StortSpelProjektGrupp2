#include "ItemSelector.h"
#include "../../game.h"
#include "../UserInterface.h"

Vector2 ItemSelector::size = Vector2(512, 128);

ItemSelector::ItemSelector(Vector2 position) : Element(position), selectedType(0)
{
	for (int i = 0; i < ItemType::TYPES_SIZE; i++)
	{
		this->selectedIndex[i] = 0;
		this->startIndex[i] = 0;
	}

	Game::getGraphics().loadTexture("UI/itemSelectorBG");
	Game::getGraphics().loadTexture("UI/itemSelectorIndicator");
	this->textureBG = Game::getGraphics().getTexturePointer("UI/itemSelectorBG");
	this->textureIndicator = Game::getGraphics().getTexturePointer("UI/itemSelectorIndicator");
	assert(textureBG && "Texture failed to load!");
	assert(textureIndicator && "Texture failed to load!");
}

ItemSelector::~ItemSelector()
{
}

void ItemSelector::draw(bool selected)
{
	UserInterface::getSpriteBatch()->Draw(this->textureBG->getShaderResView(), this->position);
	UserInterface::getSpriteBatch()->Draw(this->textureIndicator->getShaderResView(), this->position + Vector2(95.0f + 96.0f * (this->selectedIndex[this->selectedType] - this->startIndex[this->selectedType]), 47.0f));

	std::vector<Item*>* list = Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType));

	for (int i = 0; i < list->size(); i++)
	{
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), (*list)[i]->getName(), this->position + Vector2(95.0f + 96.0f * i, 47.0f + 24.0f), Colors::Red, 0, Vector2(), 0.2f);
	}
}

void ItemSelector::changeSelectedType(bool down)
{
	if (down)
	{
		this->selectedType = (this->selectedType + 1) % ItemType::TYPES_SIZE;
	}
	else
	{
		this->selectedType -= 1;

		if (this->selectedType < 0)
		{
			this->selectedType = ItemType::TYPES_SIZE - 1;
		}
	}
}

void ItemSelector::changeSelectedIndex(bool right)
{
	int listSize = Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType))->size();
	if (listSize == 0)
	{ 
		this->selectedIndex[this->selectedType] = 0;
		return;
	}
	
	if (right)
	{
		this->selectedIndex[this->selectedType] = (this->selectedIndex[this->selectedType] + 1) % listSize;
	}
	else
	{
		this->selectedIndex[this->selectedType] -= 1;

		if (this->selectedIndex[this->selectedType] < 0)
		{
			this->selectedIndex[this->selectedType] = listSize - 1;
		}
	}

	if (this->selectedIndex[this->selectedType] < this->startIndex[this->selectedType])
	{
		this->startIndex[this->selectedType] = this->selectedIndex[this->selectedType];
	}
	else if (this->selectedIndex[this->selectedType] >= this->startIndex[this->selectedType] + ItemSelector::tileLength)
	{
		this->startIndex[this->selectedType] = this->selectedIndex[this->selectedType] - (ItemSelector::tileLength - 1);
	}
}

ItemType ItemSelector::getSelectedType() const
{
	return static_cast<ItemType>(this->selectedType);
}

Item* ItemSelector::getSelectedItem() const
{
	return Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType))->at(this->selectedIndex[this->selectedType]);
}
