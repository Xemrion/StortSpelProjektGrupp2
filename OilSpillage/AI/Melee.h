#pragma once
#include "..//Weapon.h"

class Melee
{
private:
	Vector3* positionPtr;
	Vector3* velocityPtr;
	float* deltaTimePtr;
protected:
	Weapon weapon;
	Bullet bullet;
	float attackCooldown = 1;
	float curCooldown;
	void meleeAttack();
public:
	Melee();
	Melee(Vector3* pos,Vector3* velocity, float* deltaTimePtr);
	virtual ~Melee();
};