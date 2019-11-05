#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <array>
#include "../GameObject.h"

enum ItemType
{
	WEAPON,
	GADGET,
	CHASSI,
	WHEEL,
	TYPES_SIZE,
	TYPE_INVALID
};

class Item
{
private:
	static std::array<std::unique_ptr<Item>, 4> allItems;

	const char* name;
	const char* description;
	ItemType type;
	GameObject* object;
public:
	static void init();
	static Item* getItemByName(const char* name);
	static Item* getRandom();

	Item();
	Item(const char* name, const char* description, ItemType type, GameObject* object);
	virtual ~Item();

	const char* getName() const;
	const char* getDescription() const;
	ItemType getType() const;
	GameObject* getObject() const;
};

#endif // !ITEM_H
