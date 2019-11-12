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
	Flamethrower,
	aiMachineGun,
	None
};

struct Weapon
{
	int damage = 40;
	float fireRate = 3.0f;
	float bulletSpeed = 24.0f;
	float bulletLifetime = 2.0f;
	DirectX::SimpleMath::Vector3 bulletScale = DirectX::SimpleMath::Vector3(1.0);
	float spreadRadians = 0.0;
	float maxSpread = 1.0;
	float spreadIncreasePerShot = 1.0;
	float currentSpreadIncrease = 0.0;
	WeaponType type = WeaponType::Default;
	float soundTimer = 0.0f;
	float timeSinceLastShot = 0.0f;
	bool flameBool = false;
	void updateWeapon(float deltaTime)
	{
		if (deltaTime > 0.01f) {
			soundTimer += 100.0f * deltaTime;
		}
		else {
			soundTimer += 100.0f;
		}
		timeSinceLastShot += deltaTime;
		currentSpreadIncrease = max(currentSpreadIncrease - deltaTime * maxSpread * 2.0, 0.0);
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
		Weapon(), //default gun
		{     6,      0.05f,        55.0f,          1.4f,		Vector3(0.07f, 0.07f, 0.3f),	 0.1f, 0.3f, 0.11f, 0.0f, WeaponType::MachineGun },
		{    200,      1.5f,         13.0f,          3.0f,		Vector3(1.0f, 1.0f, 1.0f),		 1.0f, 0.0f, 0.0f, 0.0f, WeaponType::MissileLauncher },
		{    160,      1.0f,          0.0f,          0.5f,		Vector3(1.0f, 1.0f, 10.0f),	     1.0f, 0.0f, 0.0f, 0.0f, WeaponType::Laser },
		{     4,      0.01f,        8.0f,          1.3f,		Vector3(1.0f, 1.0f, 1.0f),		 0.2f, 0.0f, 0.0f, 0.0f, WeaponType::Flamethrower },
		{	   2,      0.3f,         12.0f,          1.0f,	    Vector3(0.2f, 0.2f, 0.2f),       0.2f, 0.0f, 0.0f, 0.0f, WeaponType::aiMachineGun }
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
				std::wstring soundEffect = L"data/sound/MachineGunSound" + std::to_wstring(randomSound) + L".wav";
				if (rand2 < 1) {
					soundEffect = L"data/sound/MachineGunSound1.wav";
				}
				Sound::PlaySoundEffect(soundEffect);
				weapon.soundTimer = 0;
			}
		}
		else if (weapon.type == WeaponType::Flamethrower)
		{
			if (weapon.flameBool == true) {
				int randomSound = rand() % 2 + 1;
				std::wstring soundEffect = L"data/sound/FlameLoop" + std::to_wstring(randomSound) + L".wav";
				Sound::PlayLoopingSound(soundEffect);
				Sound::PlaySoundEffect(L"data/sound/FlameStart.wav");
				weapon.flameBool = false;
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
			Sound::StopLoopingSound(L"data/sound/FlameLoop1.wav", true);
			Sound::StopLoopingSound(L"data/sound/FlameLoop2.wav", true);
		}
	};


};

class Bullet
{
	void defaultShoot(Weapon& weapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity);
	void defaultUpdate(float& deltaTime);
	void defaultEnemyUpdate(float& deltaTime);
	void flamethrowerShoot(Weapon& weapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity);
	GameObject* obj = nullptr;
	Vector3 dir;
	float timeLeft = 0.0f;
	WeaponType weaponType = WeaponType::None;
	static float soundTimer;
public:
	Bullet();
	Bullet(WeaponType type);
	~Bullet();
	void setWeaponType(WeaponType type);
	WeaponType getWeaponType() const;
	int getDamage() const;
	void shoot(Weapon& weapon, Vector3 position, Vector3 direction, Vector3 additionalVelocity);
	void update(float deltaTime);
	float getTimeLeft() const;
	void destroy();
	GameObject* getGameObject();
	static void updateSoundTimer(float deltaTime);
};

#endif // !WEAPON_H