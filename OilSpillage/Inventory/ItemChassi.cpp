#include "ItemChassi.h"
#include <sstream>
#include "../game.h"
std::string ItemChassi::generateDescription(Stats chassi)
{
	std::stringstream stream;
	stream << "Durability: " << chassi.maxHealth << "\n";
	stream << "Top Speed: " << chassi.speed << "\n";

	return stream.str();
}

ItemChassi::ItemChassi(std::string name, GameObject* object) : Item(name, generateDescription(stats), ItemType::CHASSI, object)
{
	this->stats = stats;
}

ItemChassi::~ItemChassi()
{
}

ItemChassi::ItemChassi(const ItemChassi& obj) : Item(obj)
{
	this->stats = obj.stats;
}

Item* ItemChassi::clone() const
{
	return new ItemChassi(*this);
}

bool ItemChassi::operator==(const ItemChassi& other) const
{
	return Item::operator==(other) /*&& this->weapon == other.weapon*/;
}

void ItemChassi::randomize()
{

	this->stats.maxHealth = static_cast<int>((((rand() % 100 +1)) + (10 * Game::getLocalScale())));
	this->stats.speed = ((rand() % 100 + 1)*0.01f) + (1 * Game::getLocalScale());
	

	this->description = generateDescription(this->stats);
	Item::randomize();
}

Stats& ItemChassi::getChassi()
{
	return this->stats;
}

Stats ItemChassi::getStats()
{
	return this->stats;
}
