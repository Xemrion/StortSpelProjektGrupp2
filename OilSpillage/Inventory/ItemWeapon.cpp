#include "ItemWeapon.h"
#include <sstream>

std::string ItemWeapon::generateDescription(Weapon weapon)
{
	std::stringstream stream;
	stream << "Damage: " << weapon.damage << "\n";
	stream << "Fire Rate: " << weapon.fireRate << "\n";
	stream << "Bullet Speed: " << weapon.bulletSpeed << "\n";
	stream << "Bullet Lifetime: " << weapon.bulletLifetime << "\n";
	stream << "Spread: " << weapon.spreadRadians << "\n";
	stream << "Max Spread: " << weapon.maxSpread << "\n";
	stream << "Spread Increase: " << weapon.spreadIncreasePerShot;

	return stream.str();
}

ItemWeapon::ItemWeapon(std::string name, Weapon weapon, GameObject* object) : Item(name, generateDescription(weapon), ItemType::WEAPON, object), weapon(weapon)
{
}

ItemWeapon::~ItemWeapon()
{
}

Weapon ItemWeapon::getWeapon() const
{
	return this->weapon;
}
