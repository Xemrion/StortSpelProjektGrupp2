#pragma once
#include "..//Weapon.h"
#include "Behaviour.h"

class Ranged
{
private:
	Vector3* positionPtr;
	Vector3* targetPosPtr;
	Vector3* velocityPtr;
	float* deltaTimePtr;


	void assignWeapon(int weaponType);
protected:
	static const int bulletCount = 32;
	Bullet bullets[bulletCount];
	float timeSinceLastShot;
	bool predicting = false;
	Weapon weapon;
	int attackRange;
	void updateBullets(float deltaTime);
	virtual Status shoot();
	Status inAttackRange();

public:
	Ranged();
	Ranged(Vector3* pos, Vector3* targetPos, Vector3* velocity, float* deltaTimePtr, int weaponType);
	virtual ~Ranged();
};