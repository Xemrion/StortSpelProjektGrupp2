#ifndef ITEM_H
#define ITEM_H

#include <string>
#include "../GameObject.h"

enum ItemType
{
	WEAPON,
	USELESS_THING,
	TYPES_SIZE
};

class Item
{
private:
	const char* name;
	ItemType type;
	GameObject* object;
public:
	Item(const char* name, ItemType type, GameObject* object);
	virtual ~Item();

	const char* getName() const;
	ItemType getType() const;
	GameObject* getObject() const;
};

#endif // !ITEM_H
