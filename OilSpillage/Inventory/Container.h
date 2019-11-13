#ifndef CONTAINER_H
#define CONTAINER_H

#include "Item.h"
#include <vector>

class Container
{
public:
	struct Slot
	{
		Container* owner = nullptr;
		Item* item = nullptr;
	};

	static std::unique_ptr<Container> playerInventory;
private:
	std::vector<Slot*> itemLists[ItemType::TYPES_SIZE];
public:
	Container();
	virtual ~Container();

	void addItem(Item* item);
	void removeItem(Slot* slot);
	bool containsItem(Slot* slot);
	bool containsItem(Item* item);
	std::vector<Slot*>* getItemStack(ItemType type);
	Slot* getItemSlot(ItemType type, int index);
};

#endif // !CONTAINER_H
