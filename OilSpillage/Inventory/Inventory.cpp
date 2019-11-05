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
	this->itemLists[item->getType()].push_back(item);
}

void Inventory::removeItem(Item* item)
{
	auto found = std::find(this->itemLists[item->getType()].begin(), this->itemLists[item->getType()].end(), item);
	
	if (found != this->itemLists[item->getType()].end())
	{
		this->itemLists[item->getType()].erase(found);
	}
}

std::vector<Item*>* Inventory::getItemList(ItemType type)
{
	return &this->itemLists[type];
}
