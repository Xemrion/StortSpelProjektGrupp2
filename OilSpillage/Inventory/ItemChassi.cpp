#include "ItemChassi.h"
#include <sstream>
#include "../game.h"

std::string ItemChassi::generateDescription(float maxHealth, float speed)
{
	std::stringstream stream;
	stream << "Max Health: " << maxHealth << "\n";
	stream << "Top Speed: " << speed << "\n";

	return stream.str();
}

ItemChassi::ItemChassi(std::string name, float maxHealth, float speed, GameObject* object) 
	: Item(name, generateDescription(maxHealth, speed), ItemType::TYPE_CHASSI, object), maxHealth(maxHealth), speed(speed)
{
}

ItemChassi::~ItemChassi()
{
}

ItemChassi::ItemChassi(const ItemChassi& obj) : Item(obj)
{
	this->maxHealth = obj.maxHealth;
	this->speed = obj.speed;
}

Item* ItemChassi::clone() const
{
	return new ItemChassi(*this);
}

void ItemChassi::randomize()
{
	this->maxHealth = static_cast<int>((((rand() % 100 +1)) + (10 * Game::getLocalScale())));
	this->speed = ((rand() % 100 + 1)*0.01f) + (1 * Game::getLocalScale());
	
	this->description = generateDescription(this->maxHealth, this->speed);
	Item::randomize();
}

float ItemChassi::getMaxHealth() const
{
	return this->maxHealth;
}

float ItemChassi::getSpeed() const
{
	return this->speed;
}
