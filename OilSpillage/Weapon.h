#ifndef WEAPON_H
#define WEAPON_H
#include <d3d11.h>
#include "SimpleMath.h"
#include "GameObject.h"
#include "Sound.h"
enum class WeaponType
{
	Default,
	MachineGun,
	MissileLauncher,
	Laser,
	Railgun,
	Flamethrower,
	Spikes,
	aiMachineGun,
	aiMelee,
	aiMissileLauncher,
	aiLaser,
	aiFlamethrower,
	None
};

struct Weapon
{
	float damage = 40;
	float fireRate = 3.0f;
	float bulletSpeed = 24.0f;
	float bulletLifetime = 2.0f;
	DirectX::SimpleMath::Vector3 bulletScale = DirectX::SimpleMath::Vector3(1.0);
	float spreadRadians = 0.0;
	float maxSpread = 1.0;
	float spreadIncreasePerSecond = 1.0; // Spread increase per shot in percentage of max spread
	float currentSpreadIncrease = 0.0;
	float spreadDecreasePerSecond = 1.0; // Spread increase per shot in percentage of max spread
	float remainingCooldown = 0.0;
	WeaponType type = WeaponType::Default;
	bool melee = false;
	float soundTimer = 0.0f;
	float timeSinceLastShot = 0.0f;
	bool flameBool = false;
	int soundHandle = 0;
	void updateWeapon(float deltaTime)
	{
		if (deltaTime > 0.01f) {
			soundTimer += 100.0f * deltaTime;
		}
		else {
			soundTimer += 100.0f;
		}
		timeSinceLastShot += deltaTime;
		currentSpreadIncrease = max(currentSpreadIncrease - deltaTime * spreadDecreasePerSecond * maxSpread, 0.0);
		remainingCooldown = max(remainingCooldown - deltaTime, 0.0);
	};

	bool updateFireRate()
	{
		if (timeSinceLastShot >= fireRate)
		{
			timeSinceLastShot = fmod(timeSinceLastShot, fireRate);
			return true;
		}
		return false;
	};
};

/*
Grattis! Här får ni en uppgift!

laserLight->setLuminance(0.0);
	for (int i = 0; i < Vehicle::bulletCount; i++)
	{
		if (bullets[i].getWeaponType() == WeaponType::Laser)
		{
			bullets[i].getGameObject()->setPosition(this->vehicleBody1->getPosition());
			bullets[i].setDirection(Vector3(curDir.x, 0, curDir.y));

			laserLight->setPos(this->mountedWeapon->getPosition() + Vector3(curDir.x, 0.0, curDir.y) * 2.0);

			laserLight->setDirection(Vector3(curDir.x, 0.0, curDir.y));
			if (this->weapon.remainingCooldown == 0.0)
			{
				laserLight->setLuminance(10.0);
			}
			laserLight->setLength(bullets[i].getWeapon().bulletScale.z);
			laserLight->setColor(Vector3::Lerp(Vector3(1.0, 0.25, 0.05), Vector3(0.2, 0.01, 0.01), (this->weapon.currentSpreadIncrease * this->weapon.currentSpreadIncrease + 0.0) / (this->weapon.maxSpread * this->weapon.maxSpread)));
		}
		else if (bullets->getMelee() == true)
		{
			bullets[i].getGameObject()->setPosition(this->vehicleBody1->getPosition());
			bullets[i].setDirection(Vector3(curDir.x, 0, curDir.y));
		}
		bullets[i].update(deltaTime);
	}

	this->spotLight->setPos(this->mountedWeapon->getPosition() - Vector3(curDir.x, -1, curDir.y));
	this->spotLight->setDirection(Vector3(curDir.x, 0, curDir.y));
*/

class WeaponHandler
{
public:
	static constexpr Weapon weapons[] = {
		//damage	fireRate	bulletSpeed		bulletLifetime	bulletScale			spreadRadians maxSpread spreadIncreasePerSecond currentSpreadIncrease spreadDecreasePerSecond  remainingCooldown WeaponType melee 
		Weapon(), //default gun
		{      6,      0.05f,        55.0f,          1.4f,		Vector3(0.07f, 0.07f, 0.3f),	 0.1f,  0.3f,  0.11f,  0.0f,  0.7f,  0.0,  WeaponType::MachineGun },
		{    10,       1.5f,        13.0f,          3.0f,		Vector3(1.0f, 1.0f, 1.0f),		 1.0f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::MissileLauncher },
		{      1,     0.015f,         0.0f,         0.15f,		Vector3(0.37f, 0.37f, 30.0f),	 0.0f,  5.0f,  1.75f,  0.0f,  1.5f,  0.0,  WeaponType::Laser },
		{    300,       1.5f,         0.0f,         0.15f,		Vector3(0.17f, 0.17f, 30.0f),	 0.0f,  0.0f,  0.11f,  0.0f,  2.0f,  0.0,  WeaponType::Railgun },
		{      4,      0.01f,         8.0f,          1.3f,		Vector3(1.0f, 1.0f, 1.0f),		 0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::Flamethrower },
		{      5,      0.005f,         8.0f,          0.005f,	Vector3(1.0f, 1.0f, 1.0f),		 0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::Spikes ,true },
		{	   0,       0.3f,        12.0f,          1.0f,	    Vector3(0.2f, 0.2f, 0.2f),       0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::aiMachineGun },
		{	  0,       0.3f,        12.0f,          1.0f,	    Vector3(0.2f, 0.2f, 0.2f),       0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::aiMelee },
		{    10,       1.5f,         4.0f,          3.0f,		Vector3(1.0f, 1.0f, 1.0f),		 1.0f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0f, WeaponType::aiMissileLauncher },
		{     1,       1.0f,         0.0f,          0.15f,		Vector3(1.0f, 1.0f, 10.0f),	     1.0f,  5.0f,  1.5f,  0.0f,  2.0f,  0.0f, WeaponType::aiLaser },
		{      1,      0.05f,         8.0f,          1.3f,		Vector3(1.0f, 1.0f, 1.0f),		 0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0f, WeaponType::aiFlamethrower },
		{0, 0, 0, 0, Vector3(0,0,0), 0, 0, 0, 0, 0, 0, WeaponType::None}

	};

	static Weapon getWeapon(WeaponType type) {
		return weapons[(int)type];
	};

	static void weaponStartSound(Weapon& weapon)
	{
		if (weapon.type == WeaponType::MachineGun)
		{
			if (weapon.soundTimer > 4.0f) {
				int randomSound = rand() % 6 + 1;
				int rand2 = rand() % 2;
				std::string soundEffect = "./data/sound/MachineGunSound" + std::to_string(randomSound) + ".wav";
				if (rand2 < 1) {
					soundEffect = "./data/sound/MachineGunSound1.wav";
				}
				Sound::play(soundEffect,0.5f);
				weapon.soundTimer = 0;
			}
		}
		else if (weapon.type == WeaponType::Flamethrower)
		{
			if (weapon.flameBool == true) {
				int randomSound = rand() % 2 + 1;
				std::string soundEffect = "./data/sound/FlameLoop" + std::to_string(randomSound) + ".wav";
				Sound::stopLooping(weapon.soundHandle);
				weapon.soundHandle = Sound::playLooping(soundEffect);
				Sound::play("./data/sound/FlameStart.wav");
				weapon.flameBool = false;
			}
		}
		else if (weapon.type == WeaponType::Laser && weapon.flameBool == true)
		{
			int randomSound = rand() % 4 + 1;
			std::string soundEffect = "./data/sound/Lazer" + std::to_string(randomSound) + ".mp3";
			Sound::stopLooping(weapon.soundHandle);
			weapon.soundHandle = Sound::playLooping(soundEffect, 0.5f);
			Sound::play("./data/sound/LazerImpact.mp3", 0.75f);
			weapon.flameBool = false;
		}
	};

	static void weaponEndSound(Weapon& weapon)
	{
		if (weapon.type == WeaponType::MachineGun)
		{
			return;
		}
		else if (weapon.type == WeaponType::Flamethrower)
		{
			weapon.flameBool = true;
			Sound::stopLooping(weapon.soundHandle);
			weapon.soundHandle = 0;
		}
		else if (weapon.type == WeaponType::Laser) 
		{
			weapon.flameBool = true;
			Sound::stopLooping(weapon.soundHandle);
			weapon.soundHandle = 0;
		}
	};


};

class Bullet
{
	void defaultShoot(Weapon& vehicleWeapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity, float deltaTime);
	void meleeShoot(Vector3& position, Vector3& direction);
	void defaultUpdate(float& deltaTime);
	void defaultEnemyUpdate(float& deltaTime);
	void enemyMeleeUpdate(float& deltaTime);
	void laserEnemyUpdate(float& deltaTime);
	void flamethrowerShoot(Weapon& vehicleWeapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity, float deltaTime);
	void laserShoot(Weapon& vehicleWeapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity, float deltaTime);
	void laserUpdate(float& deltaTime);
	GameObject* obj = nullptr;
	Vector3 dir;
	Vector3 initDir;
	Vector3 initPos;
	float timeLeft = 0.0f;
	Weapon weapon;
	static float soundTimer;
public:
	Bullet();
	Bullet(Weapon weapon);
	~Bullet();
	void setWeapon(Weapon weapon);
	Weapon getWeapon() const;
	WeaponType getWeaponType() const;
	int getDamage() const;
	bool getMelee() const;
	void shoot(Weapon& weapon, Vector3 position, Vector3 direction, Vector3 additionalVelocity, float deltaTime);
	void update(float deltaTime);
	float getTimeLeft() const;
	void destroy();
	GameObject* getGameObject();
	static void updateSoundTimer(float deltaTime);
	Vector3 getDirection() const;
	void setDirection(Vector3 newDir);
};

#endif // !WEAPON_H