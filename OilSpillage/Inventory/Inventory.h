#ifndef INVENTORY_H
#define INVENTORY_H

#include "Item.h"
#include <vector>

class Inventory
{
public:
	static std::unique_ptr<Inventory> instance;
private:
	std::vector<Item*> itemLists[ItemType::TYPES_SIZE];
	//bool changed[ItemType::TYPES_SIZE];
public:
	Inventory();
	virtual ~Inventory();

	void addItem(Item* item);
	void removeItem(Item* item);
	bool containsItem(Item* item);
	//bool hasChanged(ItemType type, bool shouldReset);
	std::vector<Item*>* getItemList(ItemType type);
};

#endif // !INVENTORY_H
