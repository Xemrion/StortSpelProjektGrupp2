#ifndef WEAPON_H
#define WEAPON_H
#include <d3d11.h>
#include "SimpleMath.h"
#include "GameObject.h"


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
};

class WeaponHandler
{
public:
	static constexpr Weapon weapons[] = {
		Weapon(), //default gun
		{     5,      0.05f,        55.0f,          2.0f,		Vector3(0.07f, 0.07f, 0.3f),	 0.1f, 0.3f, 0.11f, 0.0f, WeaponType::MachineGun },
		{    200,      1.5f,         13.0f,          3.0f,		Vector3(1.0f, 1.0f, 1.0f),		 1.0f, 0.0f, 0.0f, 0.0f, WeaponType::MissileLauncher },
		{    160,      1.0f,          0.0f,          0.5f,		Vector3(1.0f, 1.0f, 10.0f),	     1.0f, 0.0f, 0.0f, 0.0f, WeaponType::Laser },
		{     3,      0.01f,        8.0f,          1.3f,		Vector3(1.0f, 1.0f, 1.0f),		 0.2f, 0.0f, 0.0f, 0.0f, WeaponType::Flamethrower },
		{	   2,      0.3f,         12.0f,          1.0f,	    Vector3(0.2f, 0.2f, 0.2f),       0.2f, 0.0f, 0.0f, 0.0f, WeaponType::aiMachineGun }
	};

	static Weapon getWeapon(WeaponType type) { 
		return weapons[(int)type];
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