#include "ItemWheel.h"
#include <sstream>
#include "../game.h"
std::string ItemWheel::generateDescription(Stats wheel)
{
	std::stringstream stream;
	stream << "Acceleration: " << wheel.accelerationRate << "\n";
	stream << "Handling: " << wheel.handlingRate << "\n";

	return stream.str();
}

ItemWheel::ItemWheel(std::string name, GameObject* object) : Item(name, generateDescription(stats), ItemType::WHEEL, object)
{
	this->stats = stats;
}

ItemWheel::~ItemWheel()
{
}

ItemWheel::ItemWheel(const ItemWheel& obj) : Item(obj)
{
	this->stats = obj.stats;
}

Item* ItemWheel::clone() const
{
	return new ItemWheel(*this);
}

bool ItemWheel::operator==(const ItemWheel& other) const
{
	return Item::operator==(other) /*&& this->weapon == other.weapon*/;
}

void ItemWheel::randomize()
{

	this->stats.handlingRate = ((rand() % 100 + 1) * 0.01f) + (1 * Game::getLocalScale());
	this->stats.accelerationRate = ((rand() % 100 + 1) * 0.01f) + (1 * Game::getLocalScale());


	this->description = generateDescription(this->stats);
	Item::randomize();
}

Stats& ItemWheel::getWheel()
{
	return this->stats;
}

Stats ItemWheel::getStats()
{
	return this->stats;
}
