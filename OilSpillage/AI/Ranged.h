#pragma once
#include "..//Weapon.h"
#include "Behaviour.h"

class Ranged
{
private:
	Vector3* positionPtr;
	Vector3* targetPosPtr;
	Vector3* velocityPtr;
public:
	Ranged();
	Ranged(Vector3* pos, Vector3* targetPos, Vector3* velocity, int weaponType);
protected:
	static const int bulletCount = 32;
	bool predicting = false;
	int attackRange = 8;

	float timeSinceLastShot;
	Bullet bullets[bulletCount];
	virtual void  updateBullets(float deltaTime);
	virtual Status inAttackRange();
	virtual void assignWeapon(int weaponType);
	Weapon weapon;
	virtual Status shoot();

};