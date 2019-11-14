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
	Railgun,
	Flamethrower,
	aiMachineGun,
	aiMelee,
	aiMissileLauncher,
	aiLaser,
	aiFlamethrower,
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
	float spreadIncreasePerSecond = 1.0;
	float currentSpreadIncrease = 0.0;
	//float recoilDissipationTime = 1.0; // time (in seconds) for recoil to go from 100% to 0%
	float spreadDecreasePerSecond = 1.0;
	float remainingCooldown = 0.0;
	WeaponType type = WeaponType::Default;
};

class WeaponHandler
{
public:
	static constexpr Weapon weapons[] = {
		Weapon(), //default gun
		{      6,      0.05f,        55.0f,          1.4f,		Vector3(0.07f, 0.07f, 0.3f),	 0.1f,  0.3f,  0.11f,  0.0f,  0.7f,  0.0,  WeaponType::MachineGun },
		{    200,       1.5f,        13.0f,          3.0f,		Vector3(1.0f, 1.0f, 1.0f),		 1.0f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::MissileLauncher },
		{      1,     0.015f,         0.0f,         0.15f,		Vector3(0.37f, 0.37f, 30.0f),	 0.0f,  5.0f,  1.75f,  0.0f,  1.5f,  0.0,  WeaponType::Laser },
		{    300,       1.5f,         0.0f,         0.15f,		Vector3(0.17f, 0.17f, 30.0f),	 0.0f,  0.0f,  0.11f,  0.0f,  2.0f,  0.0,  WeaponType::Railgun },
		{      4,      0.01f,         8.0f,          1.3f,		Vector3(1.0f, 1.0f, 1.0f),		 0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::Flamethrower },
		{	   2,       0.3f,        12.0f,          1.0f,	    Vector3(0.2f, 0.2f, 0.2f),       0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::aiMachineGun },
		{	   2,       0.3f,        12.0f,          1.0f,	    Vector3(0.2f, 0.2f, 0.2f),       0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0,  WeaponType::aiMelee },
		{    200,       1.5f,         4.0f,          3.0f,		Vector3(1.0f, 1.0f, 1.0f),		 1.0f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0f, WeaponType::aiMissileLauncher },
		{     60,       1.0f,         0.0f,          0.5f,		Vector3(1.0f, 1.0f, 10.0f),	     1.0f,  5.0f,  1.75f,  0.0f,  2.0f,  0.0f, WeaponType::aiLaser },
		{      1,      0.05f,         8.0f,          1.3f,		Vector3(1.0f, 1.0f, 1.0f),		 0.2f,  0.0f,   0.0f,  0.0f,  2.0f,  0.0f, WeaponType::aiFlamethrower },
		{0, 0, 0, 0, Vector3(0,0,0), 0, 0, 0, 0, 0, 0, WeaponType::None}
	};

	static Weapon getWeapon(WeaponType type) {
		return weapons[(int)type];
	};
};

class Bullet
{
	void defaultShoot(Weapon& vehicleWeapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity, float deltaTime);
	void defaultUpdate(float& deltaTime);
	void defaultEnemyUpdate(float& deltaTime);
	void flamethrowerShoot(Weapon& vehicleWeapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity, float deltaTime);
	void laserShoot(Weapon& vehicleWeapon, Vector3& position, Vector3& direction, Vector3& additionalVelocity, float deltaTime);
	void laserUpdate(float& deltaTime);
	GameObject* obj = nullptr;
	Vector3 dir;
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