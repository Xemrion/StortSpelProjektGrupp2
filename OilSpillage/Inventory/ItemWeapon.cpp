#include "ItemWeapon.h"
#include <sstream>

std::string ItemWeapon::generateDescription(Weapon weapon)
{
	std::stringstream stream;
	stream << "Damage: " << weapon.damage << "\n";
	stream << "Fire Rate: " << weapon.fireRate << "\n";
	stream << "Bullet Speed: " << weapon.bulletSpeed << "\n";
	stream << "Max Spread: " << weapon.maxSpread << "\n";
	
	return stream.str();
}

ItemWeapon::ItemWeapon(std::string name, Weapon weapon, std::shared_ptr<GameObject> object) : Item(name, generateDescription(weapon), ItemType::WEAPON, object), weapon(weapon)
{

}

ItemWeapon::~ItemWeapon()
{

}

ItemWeapon::ItemWeapon(const ItemWeapon& obj) : Item(obj)
{
	this->weapon = obj.weapon;
}

Item* ItemWeapon::clone() const
{
	return new ItemWeapon(*this);
}

Weapon& ItemWeapon::getWeapon()
{
	return this->weapon;
}
