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

ItemWeapon::ItemWeapon(std::string name, Weapon weapon, GameObject * object) : Item(name, generateDescription(weapon), ItemType::WEAPON, object), weapon(weapon)
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

void ItemWeapon::randomize()
{
	//damage
	//fireRate
	//bulletSpeed
	//bulletLifetime
	//spreadRadians
	//maxSpread
	//spreadIncreasePerSecond
	//spreadDecreasePerSecond

	this->weapon.damage =  static_cast<int>(this->weapon.damage						* (rand() % 1001 / 500.0f)) + 1;
	this->weapon.bulletSpeed =				this->weapon.bulletSpeed				* (rand() % 1001 / 500.0f) + 0.1f;
	this->weapon.bulletLifetime =			this->weapon.bulletLifetime				* (rand() % 1001 / 500.0f) + 0.1f;
	this->weapon.spreadRadians =			this->weapon.spreadRadians				* (rand() % 1001 / 500.0f) + 0.1f;
	this->weapon.maxSpread =				this->weapon.maxSpread					* (rand() % 1001 / 500.0f) + 0.1f;
	this->weapon.spreadIncreasePerSecond =	this->weapon.spreadIncreasePerSecond	* (rand() % 1001 / 500.0f) + 0.1f;
	this->weapon.spreadDecreasePerSecond =	this->weapon.spreadDecreasePerSecond	* (rand() % 1001 / 500.0f) + 0.1f;

	this->description = generateDescription(this->weapon);
	Item::randomize();
}

Weapon& ItemWeapon::getWeapon()
{
	return this->weapon;
}
