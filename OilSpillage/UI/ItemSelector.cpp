#include "ItemSelector.h"

Vector2 ItemSelector::size = Vector2();

ItemSelector::ItemSelector(Vector2 position) : Element(position), selectedType(0)
{
	for (int i = 0; i < ItemType::TYPES_SIZE; i++)
	{
		selectedIndex[i] = 0;
	}
}

ItemSelector::~ItemSelector()
{
}

void ItemSelector::draw(bool selected)
{
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

void ItemSelector::changeSelectedIndex(bool left)
{
	int stackSize = Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType))->size();
	
	if (!left)
	{
		this->selectedIndex[this->selectedType] = (this->selectedIndex[this->selectedType] + 1) % stackSize;
	}
	else
	{
		this->selectedIndex[this->selectedType] -= 1;

		if (this->selectedIndex[this->selectedType] < 0)
		{
			this->selectedIndex[this->selectedType] = stackSize - 1;
		}
	}
}

Item* ItemSelector::getSelectedItem() const
{
	return Inventory::instance->getItemList(static_cast<ItemType>(this->selectedType))->at(this->selectedIndex[this->selectedType]);
}
