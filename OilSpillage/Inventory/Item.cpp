#include "Item.h"
#include "ItemWeapon.h"

std::vector<Item> Item::allItems;

Item::Item(const char* name, const char* description, ItemType type, GameObject* object)
	: name(name), description(description), type(type), object(object)
{
}

Item::~Item()
{
}

void Item::init()
{
	Item::allItems = {
		Item("Test Item", "A very useless thing!", ItemType::GADGET, nullptr),
		Item("Test Item 2", "A very useless thing 2!", ItemType::GADGET, nullptr),
		Item("Test Item 3", "A very useless thing 3!", ItemType::GADGET, nullptr),
		ItemWeapon("Machine Gun", WeaponHandler::getWeapon(WeaponType::MachineGun), nullptr)
	};

	//Sort so we can use getItemByName later if needed.
	std::sort(allItems.begin(), allItems.end(), [](const Item& a, const Item& b) { return std::strcmp(a.getName(), b.getName()); });
}

Item* Item::getItemByName(const char* name)
{
	auto item = std::lower_bound(allItems.begin(), allItems.end(), name, [](const Item& item, const char* name) { return std::strcmp(item.getName(), name); });

	if (item != allItems.end())
	{
		return &(*item);
	}

	return nullptr;
}

Item* Item::getRandom()
{
	return &allItems[rand() % allItems.size()];
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
