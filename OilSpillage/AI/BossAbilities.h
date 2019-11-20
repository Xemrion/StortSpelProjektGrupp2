#pragma once
#include "..//Weapon.h"
#include "Behaviour.h"

class BossAbilities
{
public:
	BossAbilities();
	BossAbilities(Vector3* pos, Vector3* targetPos, Vector3* velocity, int weaponType);
	virtual ~BossAbilities();
	BossAbilities& operator=(const BossAbilities& other);

private:
	Vector3* positionPtr;
	Vector3* targetPosPtr;
	Vector3* velocityPtr;
	WaitTimer waitTimer;

	float timeSinceLastShot;
	bool predicting = false;
	int weaponNr;
	Weapon weapon;
	static const int bulletCount = 32;
	Bullet bullets[bulletCount];

	void assignWeapon(int weaponType);
protected:
	int attackRange;
	float dt;
	Vector3 targetToSelf; //use in boll abilities

	Status shoot();
	Status ability1();
	Status switchWeapon();
	Status waitForStart();

	void updateBullets(float deltaTime);
	Status inAttackRange();
};