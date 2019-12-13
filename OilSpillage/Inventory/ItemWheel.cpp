#include "ItemWheel.h"
#include <sstream>
#include "../game.h"

std::string ItemWheel::generateDescription(float accelerationRate, float handlingRate)
{
	std::stringstream stream;
	stream << "Acceleration: " << accelerationRate << "\n";
	stream << "Handling: " << handlingRate << "\n";

	return stream.str();
}

ItemWheel::ItemWheel(std::string name, float accelerationRate, float handlingRate, GameObject* object) 
	: Item(name, generateDescription(accelerationRate, handlingRate), ItemType::TYPE_WHEEL, object), accelerationRate(accelerationRate), handlingRate(handlingRate)
{
}

ItemWheel::~ItemWheel()
{
}

ItemWheel::ItemWheel(const ItemWheel& obj) : Item(obj)
{
	this->accelerationRate = obj.accelerationRate;
	this->handlingRate = obj.handlingRate;
}

Item* ItemWheel::clone() const
{
	return new ItemWheel(*this);
}

void ItemWheel::randomize()
{
	this->accelerationRate = this->accelerationRate * ((rand() % 200 + 1) * 0.01f) + (2 * Game::getLocalScale())-2;
	this->handlingRate = this->handlingRate * ((rand() % 200 + 1) * 0.01f) + (2 * Game::getLocalScale())-2;

	this->description = this->generateDescription(this->accelerationRate, this->handlingRate);
	Item::randomize();
}

float ItemWheel::getAccelerationRate() const
{
	return this->accelerationRate;
}

float ItemWheel::getHandlingRate() const
{
	return this->handlingRate;
}
