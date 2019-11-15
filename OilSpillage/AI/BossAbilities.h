#pragma once
#include "..//Weapon.h"
#include "Behaviour.h"

class BossAblilities
{
public:
	BossAblilities();
	BossAblilities(Vector3* pos, Vector3* targetPos, Vector3* velocity, int weaponType);
	virtual ~BossAblilities();

	Status ability1(); //should prob be protected/private but didnt work in boss.cpp then
private:
	Vector3* positionPtr;
	Vector3* targetPosPtr;
	Vector3* velocityPtr;
	float timeSinceLastShot;
	bool predicting = false;
	Weapon weapon;
	static const int bulletCount = 32;
	Bullet bullets[bulletCount];

	void assignWeapon(int weaponType);
protected:
	int attackRange;

	Status shoot();

	void updateBullets(float deltaTime);
	Status inAttackRange();
};