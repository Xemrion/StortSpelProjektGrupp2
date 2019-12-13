#pragma once
#include "..//Weapon.h"

class Melee
{
private:
	btRigidBody* positionPtr;
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
	Melee(btRigidBody* pos,Vector3* velocity, float* deltaTimePtr);
	virtual ~Melee();
};