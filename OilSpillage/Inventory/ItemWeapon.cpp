#include "ItemWeapon.h"
#include <sstream>

const char* ItemWeapon::generateDescription()
{
	std::stringstream stream;
	stream << "Damage:          " << this->weapon.damage;
	stream << "Fire Rate:       " << this->weapon.fireRate;
	stream << "Bullet Speed:    " << this->weapon.bulletSpeed;
	stream << "Bullet Lifetime: " << this->weapon.bulletLifetime;
	stream << "Spread:          " << this->weapon.spreadRadians;
	stream << "Max Spread:      " << this->weapon.maxSpread;
	stream << "Spread Increase: " << this->weapon.spreadIncreasePerShot;

	return stream.str().c_str();
}

ItemWeapon::ItemWeapon(const char* name, Weapon weapon, GameObject* object) : Item(name, this->generateDescription(), ItemType::WEAPON, object)
{
}

ItemWeapon::~ItemWeapon()
{
}

Weapon ItemWeapon::getWeapon() const
{
	return this->weapon;
}
