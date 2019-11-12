#ifndef INVENTORY_H
#define INVENTORY_H

#include "Item.h"
#include <vector>

struct InventorySlot
{
	ItemType type = ItemType::WEAPON;
	int index = -1;

	InventorySlot() : type(ItemType::WEAPON), index(-1) {}
	InventorySlot(ItemType type, int index) : type(type), index(index) {}
	bool operator==(const InventorySlot& other) const { return this->index == other.index && this->type == other.type; }
	bool operator!=(const InventorySlot& other) const { return !operator==(other); }
};

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
	Item* getItemInList(ItemType type, int index);
	Item* getItemInList(InventorySlot slot);
};

#endif // !INVENTORY_H
