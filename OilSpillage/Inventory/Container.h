#ifndef CONTAINER_H
#define CONTAINER_H

#include "Item.h"
#include <vector>

class Container
{
public:
	class Slot
	{
	private:
		Container* owner;
		Item* item;
	public:
		Slot() : owner(nullptr), item(nullptr) {}
		Slot(Container* owner, Item* item) : owner(owner), item(item) {}
		virtual ~Slot() { delete item; }

		Container* getOwner() const { return owner; }
		Item* getItem() const { return item; }
		void setOwner(Container* owner) { this->owner = owner; }
		void setItem(Item* item) { this->item = item; }
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
