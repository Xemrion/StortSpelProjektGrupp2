#include "Inventory.h"

std::unique_ptr<Inventory> Inventory::instance;

Inventory::Inventory() //: changed{false}
{
}

Inventory::~Inventory()
{
}

void Inventory::addItem(Item* item)
{
	this->itemLists[item->getType()].push_back(item);
	//this->changed[item->getType()] = true;
}

void Inventory::removeItem(Item* item)
{
	auto found = std::find(this->itemLists[item->getType()].begin(), this->itemLists[item->getType()].end(), item);
	
	if (found != this->itemLists[item->getType()].end())
	{
		this->itemLists[item->getType()].erase(found);
		//this->changed[item->getType()] = true;
	}
}

/*bool Inventory::hasChanged(ItemType type, bool shouldReset)
{
	bool temp = this->changed[type];

	if (this->changed[type] && shouldReset)
	{
		this->changed[type] = false;
	}

	return temp;
}*/

std::vector<Item*>* Inventory::getItemList(ItemType type)
{
	return &this->itemLists[type];
}
