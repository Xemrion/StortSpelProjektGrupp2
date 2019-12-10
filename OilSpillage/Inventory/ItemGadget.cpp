#include "ItemGadget.h"
#include <sstream>
#include "../game.h"

std::string ItemGadget::generateDescription(Gadget gadget)
{
	std::stringstream stream;
	stream << std::boolalpha;
	if (gadget.type == GadgetType::EMP)
	{
		stream << "lifeTime: " << gadget.lifeTime	<<	"\n";
		stream << "radius: " << gadget.radius	<<	"\n";
		stream << "cooldown: " << gadget.cooldown	<<	"\n";
	}
	else if (gadget.type == GadgetType::NITRO)
	{
		stream << "lifeTime: " << gadget.lifeTime << "\n";
	//	stream << "radius: " << gadget.radius << "\n";
		stream << "cooldown: " << gadget.cooldown << "\n";
		stream << "power: " << gadget.power << "\n";
	}
	return stream.str();
}

ItemGadget::ItemGadget(std::string name, Gadget gadget, GameObject* object)
	:Item(name, generateDescription(gadget), ItemType::TYPE_GADGET, object), gadget(gadget)
{

}

ItemGadget::~ItemGadget()
{
}

ItemGadget::ItemGadget(const ItemGadget& other): Item(other)
{
	this->gadget = other.gadget;
}

Item* ItemGadget::clone() const
{
	return new ItemGadget(*this);
}

//bool ItemGadget::operator==(const ItemGadget& other) const
//{
//	return Item::operator==(other);
//}

void ItemGadget::randomize()
{
	if (gadget.type == GadgetType::EMP)
	{
		gadget.lifeTime = random(5, 2) + (1 * Game::getLocalScale());
		gadget.radius = random(6,3) + (1 * Game::getLocalScale());
		gadget.cooldown = random(30, 15) - (1 * Game::getLocalScale());
	}
	else if (gadget.type == GadgetType::NITRO)
	{
		gadget.lifeTime = random(10,5) + (1 * Game::getLocalScale());
		//gadget.radius = random(6, 3) + (1 * Game::getLocalScale());
		gadget.cooldown = random(30, 15) - (1 * Game::getLocalScale());
		gadget.power = random(50, 5) - (1 * Game::getLocalScale());
	}
}
Gadget& ItemGadget::getGadget()
{
	return gadget;
}
