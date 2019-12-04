#ifndef WEAPON_H
#define WEAPON_H
#include <d3d11.h>
#include "SimpleMath.h"
#include "GameObject.h"
#include "Sound.h"
#include "Lights.h"

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
	aiBossFlamethrower,
	aiBossFlamethrowerPhase2,
	aiBossMachineGun,
	aiBossMachineGunPhase2,
	aiBossMissileLauncher,
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
	bool doesDoT = false;
	bool doesSplashDmg = false;
	float splashRange = 0.0;
	float soundTimer = 0.0f;
	float timeSinceLastShot = 0.0f;
	bool flameBool = false;
	float doTTimer = 0.0f;
	bool doesKnockBack = false;
	float knockbackForce = 0.0f;
	int soundHandle = 0;
	Vector3 lightColor = Vector3(0, 0, 0);
	Light* light = nullptr;

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

class WeaponHandler
{
public:
	static constexpr Weapon weapons[] = {
		//damage	fireRate	bulletSpeed		bulletLifetime	bulletScale			spreadRadians maxSpread spreadIncreasePerSecond currentSpreadIncrease spreadDecreasePerSecond  remainingCooldown WeaponType melee dot
		Weapon(), //default gun
		{      6,      0.05f,        55.0f,          1.4f,		Vector3(0.07f, 0.07f, 0.3f),	 0.1f,  0.3f,  0.11f,  0.0f,  0.7f,  0.0,  WeaponType::MachineGun },
		{     10,       1.5f,        13.0f,          3.0f,		Vector3(1.0f, 1.0f, 1.0f),		 1.0f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::MissileLauncher, false, false, true },
		{      2,     0.015f,         0.0f,         0.15f,		Vector3(0.37f, 0.37f, 50.0f),	 0.0f,  5.0f,  1.75f,  0.0f,  1.5f,  0.0,  WeaponType::Laser },
		{    300,       1.5f,         0.0f,         0.15f,		Vector3(0.17f, 0.17f, 30.0f),	 0.0f,  0.0f,  0.11f,  0.0f,  2.0f,  0.0,  WeaponType::Railgun },
		{      4,      0.01f,         8.0f,          1.3f,		Vector3(1.0f, 1.0f, 1.0f),		 0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::Flamethrower, false, true },
		{      5,      0.005f,         1.0f,          0.2f,		Vector3(1.0f, 1.0f, 1.0f),		 0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::Spikes, true },
		{	   2,       0.3f,        12.0f,          1.0f,	    Vector3(0.2f, 0.2f, 0.2f),       0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::aiMachineGun },
		{	   5,       0.3f,        12.0f,          1.0f,	    Vector3(0.2f, 0.2f, 0.2f),       0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::aiMelee },
		{     50,       1.5f,         4.0f,          3.0f,		Vector3(1.0f, 1.0f, 1.0f),		 1.0f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0f, WeaponType::aiMissileLauncher },
		{   0.33,       1.0f,         0.0f,          0.5f,		Vector3(1.0f, 1.0f, 10.0f),	     1.0f,  5.0f,   1.5f,  0.0f,  2.0f,  0.0f, WeaponType::aiLaser },
		{      1,      0.05f,         8.0f,          1.3f,		Vector3(1.0f, 1.0f, 1.0f),		 0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0f, WeaponType::aiFlamethrower, false, true },
		{      2,     0.015f,        20.0f,          1.0f,		Vector3(1.0f, 1.0f, 1.0f),		 0.0f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0f, WeaponType::aiBossFlamethrower, false, true },
		{      6,     0.015f,        25.0f,          2.0f,		Vector3(1.0f, 1.0f, 1.0f),		 0.0f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0f, WeaponType::aiBossFlamethrowerPhase2, false, true },
		{      1,     0.025f,        30.0f,          4.0f,		Vector3(1.0f, 1.0f, 1.0f),	  	 0.2f, 60.0f,  20.0f,  0.0f,  0.7f,  0.0,  WeaponType::aiBossMachineGun },
		{      3,     0.022f,        45.0f,          3.5f,		Vector3(1.0f, 1.0f, 1.0f),	  	 0.1f, 10.0f,  10.0f,  0.0f,  0.7f,  0.0,  WeaponType::aiBossMachineGunPhase2 },
		{    100,       0.5f,        40.0f,         15.0f,		Vector3(1.0f, 1.0f, 1.0f),		 1.0f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0f, WeaponType::aiBossMissileLauncher },
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
			if (weapon.remainingCooldown <= 0) {
				int randomSound = rand() % 4 + 1;
				std::string soundEffect = "./data/sound/Lazer" + std::to_string(randomSound) + ".mp3";
				Sound::stopLooping(weapon.soundHandle);
				weapon.soundHandle = Sound::playLooping(soundEffect, 0.5f);
				Sound::play("./data/sound/LazerImpact.mp3", 0.75f);
				weapon.flameBool = false;
			}
			else {
				weaponEndSound(weapon);
			}
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
	void defaultShoot(Weapon& vehicleWeapon, const Vector3& position, const Vector3& direction, const Vector3& additionalVelocity, float deltaTime);
	void defaultShootEnemy(Weapon& vehicleWeapon, const Vector3& position, const Vector3& direction, const Vector3& additionalVelocity, float deltaTime);
	void meleeShoot(const Vector3& position,const Vector3& direction);
	void defaultUpdate(float& deltaTime);
	void defaultEnemyUpdate(float& deltaTime);
	void enemyMeleeUpdate();
	void laserEnemyUpdate(float& deltaTime);
	void flamethrowerShoot(const Vector3& position, const Vector3& direction, const Vector3& additionalVelocity);
	void laserShoot(Weapon& vehicleWeapon, const Vector3& direction, float deltaTime);
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
	float getDamage() const;
	bool getMelee() const;
	bool getFlame() const;
	float getFlameTimer()const;
	bool getKnockback() const;
	float getKnockbackForce() const;
	bool getSplashBool() const;
	float getSplashRange() const;
	void shoot(Weapon& weapon, const Vector3& position, const Vector3& normalizedDir,const Vector3& additionalVelocity, float deltaTime);
	void update(float deltaTime);
	float getTimeLeft() const;
	void destroy();
	GameObject* getGameObject();
	static void updateSoundTimer(float deltaTime);
	Vector3 getDirection() const;
	void setDirection(Vector3 newDir);
};

#endif // !WEAPON_H