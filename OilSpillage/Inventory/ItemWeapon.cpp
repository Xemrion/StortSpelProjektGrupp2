#include "ItemWeapon.h"
#include <sstream>
#include "../game.h"
#include <iomanip>

std::string ItemWeapon::generateDescription(Weapon weapon)
{
	std::stringstream stream;
	stream << std::boolalpha;

	if (weapon.type == WeaponType::MachineGun) {
		stream << "Damage/S: " << Item::fixedDecimals(weapon.damage / weapon.fireRate, 2) << "\n";
		stream << "Fire Rate: " << Item::fixedDecimals(weapon.fireRate, 2) << "\n";
		stream << "Bullet Speed: " << Item::fixedDecimals(weapon.bulletSpeed, 2) << "\n";
		stream << "Max Spread: " << Item::fixedDecimals(weapon.maxSpread, 2) << "\n";
	}
	else if(weapon.type == WeaponType::Laser){
		stream << "Damage/S: " << Item::fixedDecimals(weapon.damage, 2) << "\n";
		stream << "Overheat : " << Item::fixedDecimals(weapon.maxSpread, 2) << "\n";


	}
	else if(weapon.type == WeaponType::Flamethrower) {
		stream << "Damage/S: " << Item::fixedDecimals(weapon.damage / weapon.fireRate, 2) << "\n";
		stream << "Fire Rate: " << Item::fixedDecimals(weapon.fireRate, 2) << "\n";
		stream << "Bullet Speed: " << Item::fixedDecimals(weapon.bulletSpeed, 2) << "\n";
		stream << "Max Spread: " << Item::fixedDecimals(weapon.maxSpread, 2) << "\n";
	}
	else if(weapon.type == WeaponType::Spikes){
		stream << "Damage/S: " << Item::fixedDecimals(weapon.damage, 2) << "\n";

	}
	else {
		stream << "Damage/S: " << Item::fixedDecimals(weapon.damage / weapon.fireRate, 2) << "\n";
		stream << "Fire Rate: " << Item::fixedDecimals(weapon.fireRate, 2) << "\n";
		stream << "Bullet Speed: " << Item::fixedDecimals(weapon.bulletSpeed, 2) << "\n";
		stream << "Max Spread: " << Item::fixedDecimals(weapon.maxSpread, 2) << "\n";

	}
	if (weapon.doesDoT)
	{
		stream << "DoT duration: " << Item::fixedDecimals(weapon.doTTimer, 2) << "\n";
	}
	if (weapon.doesKnockBack)
	{
		stream << "Knockback force: " << Item::fixedDecimals(weapon.knockbackForce, 2) << "\n";
	}
	if (weapon.doesSplashDmg)
	{
		stream << "Splash damage range: " << Item::fixedDecimals(weapon.splashRange, 2) << "\n";
	}
	return stream.str();
}

ItemWeapon::ItemWeapon(std::string name, Weapon weapon, GameObject * object) : Item(name, generateDescription(weapon), ItemType::TYPE_WEAPON, object), weapon(weapon)
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
	if (weapon.type == WeaponType::MachineGun) 
	{
		this->weapon.damage = static_cast<int>(this->weapon.damage * (((rand() % 400) *0.01f) + (1 * Game::getLocalScale())));
		this->weapon.bulletSpeed = this->weapon.bulletSpeed * (((rand() % 101) * 0.01f) + 1);
		this->weapon.bulletLifetime = this->weapon.bulletLifetime * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadRadians = this->weapon.spreadRadians + ((rand() % 101) *0.01f)*0.52f;//30 degree = 0.52 radians
		this->weapon.maxSpread = this->weapon.maxSpread + ((rand() % 101) * 0.01f+1.0f) * this->weapon.spreadRadians;
		this->weapon.spreadIncreasePerSecond = this->weapon.spreadIncreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadDecreasePerSecond = this->weapon.spreadDecreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		int chance = (rand() % 100) + 1;
		if(chance <= 10)
		{
			weapon.doesDoT = true;
			weapon.doTTimer = 2;
		}
		else if(chance > 10 && chance <= 20)
		{
			weapon.doesKnockBack = true;
			weapon.knockbackForce = 2;
		}
		else if (chance > 20 && chance <= 30)
		{
			weapon.doesSplashDmg = true;
			weapon.splashRange = 20;
		}

	}
	else if (weapon.type == WeaponType::Laser) 
	{
		this->weapon.damage = static_cast<int>(this->weapon.damage * (((rand() % 201+400) * 0.01f) + (1 * Game::getLocalScale())));
		this->weapon.bulletSpeed = this->weapon.bulletSpeed * 1;
		this->weapon.bulletLifetime = this->weapon.bulletLifetime * 1;
		this->weapon.spreadRadians = this->weapon.spreadRadians * (((rand() % 101) * 0.01f) + (1 * Game::getLocalScale()));
		this->weapon.maxSpread = this->weapon.maxSpread * (((rand() % 101) * 0.01f) + (1 * Game::getLocalScale()));
		this->weapon.spreadIncreasePerSecond = this->weapon.spreadIncreasePerSecond * 1/*(((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()))*/;
		this->weapon.spreadDecreasePerSecond = this->weapon.spreadDecreasePerSecond *1 /*(((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()))*/;
		int chance = (rand() % 100) + 1;
		if (chance <= 10)
		{
			weapon.doesDoT = true;
			weapon.doTTimer = 2;
		}
		else if (chance > 10 && chance <= 20)
		{
			weapon.doesKnockBack = true;
			weapon.knockbackForce = 2;
		}
		else if (chance > 20 && chance <= 30)
		{
			weapon.doesSplashDmg = true;
			weapon.splashRange = 20;
		}
	}
	else if (weapon.type == WeaponType::Flamethrower) 
	{
		this->weapon.damage = static_cast<int>(this->weapon.damage * (((rand() % 50) * 0.01f) + (1 * Game::getLocalScale())));
		this->weapon.bulletSpeed = this->weapon.bulletSpeed;// *(((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.bulletLifetime = this->weapon.bulletLifetime * (((rand() % 50)*0.01f) + (1 * Game::getLocalScale()));
		this->weapon.spreadRadians = this->weapon.spreadRadians + ((rand() % 101) * 0.01f) * 0.52f;//30 degree = 0.52 radians
		this->weapon.maxSpread = this->weapon.maxSpread + ((rand() % 101) * 0.01f + 1.0f) * this->weapon.spreadRadians;
		this->weapon.spreadIncreasePerSecond = this->weapon.spreadIncreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadDecreasePerSecond = this->weapon.spreadDecreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		int chance = (rand() % 100) + 1;
		if (chance > 10 && chance <= 20)
		{
			weapon.doesKnockBack = true;
			weapon.knockbackForce = 2;
		}
		else if (chance > 20 && chance <= 30)
		{
			weapon.doesSplashDmg = true;
			weapon.splashRange = 20;
		}
	}
	else if (weapon.type == WeaponType::Spikes) 
	{
		this->weapon.damage = static_cast<int>(this->weapon.damage * (((rand() % 500 + 200) * 0.01f) + (1 * Game::getLocalScale())));
		int chance = (rand() % 100) + 1;
		if (chance <= 10)
		{
			weapon.doesDoT = true;
			weapon.doTTimer = 2;
		}
		else if (chance > 10 && chance <= 20)
		{
			weapon.doesKnockBack = true;
			weapon.knockbackForce = 2;
		}
		else if (chance > 20 && chance <= 30)
		{
			weapon.doesSplashDmg = true;
			weapon.splashRange = 20;
		}
		/*this->weapon.bulletSpeed = this->weapon.bulletSpeed * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.bulletLifetime = this->weapon.bulletLifetime * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadRadians = this->weapon.spreadRadians * (((rand() % 1000 + 1) / 100) + (1));
		this->weapon.maxSpread = this->weapon.maxSpread * (((rand() % 1000 + 1) / 100) + (1));
		this->weapon.spreadIncreasePerSecond = this->weapon.spreadIncreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadDecreasePerSecond = this->weapon.spreadDecreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));*/
	}
	else 
	{
		this->weapon.damage = static_cast<int>(this->weapon.damage * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale())));
		this->weapon.bulletSpeed = this->weapon.bulletSpeed * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.bulletLifetime = this->weapon.bulletLifetime * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadRadians = this->weapon.spreadRadians * (((rand() % 1000 + 1) / 100) + (1));
		this->weapon.maxSpread = this->weapon.maxSpread * (((rand() % 1000 + 1) / 100) + (1));
		this->weapon.spreadIncreasePerSecond = this->weapon.spreadIncreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
		this->weapon.spreadDecreasePerSecond = this->weapon.spreadDecreasePerSecond * (((rand() % 1000 + 1) / 100) + (1 * Game::getLocalScale()));
	}
	

	this->description = generateDescription(this->weapon);
	if (this->weapon.type == WeaponType::Laser)
	{
		this->weapon.lightColor = Vector3(rand(), rand(), rand());
		this->weapon.lightColor /= max(max(this->weapon.lightColor.x, this->weapon.lightColor.y), this->weapon.lightColor.z);
	}
	Item::randomize();
}

Weapon& ItemWeapon::getWeapon()
{
	return this->weapon;
}

void ItemWeapon::update(float dt)
{
	if (weapon.type == WeaponType::Spikes)
	{

	}
}
