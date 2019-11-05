#include "Item.h"
#include "ItemWeapon.h"

std::unordered_map<const char*, Item> Item::allItems;

Item::Item(const char* name, const char* description, ItemType type, GameObject* object)
	: name(name), description(description), type(type), object(object)
{
}

Item::~Item()
{
}

void Item::init()
{
	Item::allItems["Test Item"] = Item("Test Item", "A very useless thing!", ItemType::GADGET, nullptr);
	Item::allItems["Machine Gun"] = ItemWeapon("Machine Gun", WeaponHandler::getWeapon(WeaponType::MachineGun), nullptr);
}

Item* Item::getItemByName(const char* name)
{
	auto item = allItems.find(name);

	if (item != allItems.end())
	{
		return &item->second;
	}

	return nullptr;
}

Item* Item::getRandom()
{
	return &std::next(allItems.begin(), rand() % allItems.size())->second;
}

const char* Item::getName() const
{
	return this->name;
}

const char* Item::getDescription() const
{
	return this->description;
}

ItemType Item::getType() const
{
	return this->type;
}

GameObject* Item::getObject() const
{
	return this->object;
}
