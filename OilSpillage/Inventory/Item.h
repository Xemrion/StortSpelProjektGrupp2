#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <unordered_map>
#include "../GameObject.h"

enum ItemType
{
	WEAPON,
	GADGET,
	CHASSI,
	WHEEL,
	TYPES_SIZE
};

class Item
{
private:
	static std::unordered_map<const char*, Item> allItems;

	const char* name;
	const char* description;
	ItemType type;
	GameObject* object;
protected:
	Item(const char* name, const char* description, ItemType type, GameObject* object);
	virtual ~Item();
public:
	static void init();
	static Item* getItemByName(const char* name);
	static Item* getRandom();

	const char* getName() const;
	const char* getDescription() const;
	ItemType getType() const;
	GameObject* getObject() const;
};

#endif // !ITEM_H
