#include "Inventory.h"

std::unique_ptr<Inventory> Inventory::instance;

Inventory::Inventory()
{
}

Inventory::~Inventory()
{
}

void Inventory::addItem(Item* item)
{
	this->itemStacks[item->getType()].push_back(item);
}

void Inventory::removeItem(Item* item)
{
	auto found = std::find(this->itemStacks[item->getType()].begin(), this->itemStacks[item->getType()].end(), item);
	
	if (found != this->itemStacks[item->getType()].end())
	{
		this->itemStacks[item->getType()].erase(found);
	}
}

std::vector<Item*>* Inventory::getItemList(ItemType type)
{
	return &this->itemStacks[type];
}
