#include "Item.h"
#include "ItemWeapon.h"

std::array<std::unique_ptr<Item>, 4> Item::allItems = {
	std::make_unique<Item>("Test Item", "A very useless thing!", ItemType::GADGET, nullptr),
	std::make_unique<Item>("Test Item 2", "A very useless thing 2!", ItemType::GADGET, nullptr),
	std::make_unique<Item>("Test Item 3", "A very useless thing 3!", ItemType::GADGET, nullptr),
	std::make_unique<ItemWeapon>("Machine Gun", WeaponHandler::getWeapon(WeaponType::MachineGun), nullptr)
};

Item::Item() : name(""), description(""), type(TYPE_INVALID), object(nullptr)
{
}

Item::Item(const char* name, const char* description, ItemType type, GameObject* object)
	: name(name), description(description), type(type), object(object)
{
}

Item::~Item()
{
}

void Item::init()
{
	//Sort so we can use getItemByName later if needed.
	std::sort(allItems.begin(), allItems.end(), [](const std::unique_ptr<Item>& a, const std::unique_ptr<Item>& b)
	{
		return std::strcmp(a->getName(), b->getName());
	});
}

Item* Item::getItemByName(const char* name)
{
	auto item = std::lower_bound(allItems.begin(), allItems.end(), name, [](const std::unique_ptr<Item>& item, const char* name)
	{
		return std::strcmp(item->getName(), name);
	});

	if (item != allItems.end())
	{
		return item->get();
	}

	return nullptr;
}

Item* Item::getRandom()
{
	return allItems[rand() % allItems.size()].get();
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
