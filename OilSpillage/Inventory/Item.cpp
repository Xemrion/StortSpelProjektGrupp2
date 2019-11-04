#include "Item.h"

Item::Item(const char* name, ItemType type, GameObject* object) 
	: name(name), type(type), object(object)
{
}

Item::~Item()
{
}

const char* Item::getName() const
{
	return name;
}

ItemType Item::getType() const
{
	return type;
}

GameObject* Item::getObject() const
{
	return object;
}
