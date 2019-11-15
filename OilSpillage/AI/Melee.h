#pragma once
#include "..//Weapon.h"

class Melee
{
private:
	Vector3* positionPtr;
	Vector3* targetPosPtr;
	Vector3* velocityPtr;
	float* deltaTimePtr;
protected:
	Weapon weapon;
	Bullet bullet;
	void meleeAttack();
public:
	Melee();
	Melee(Vector3* pos,Vector3* velocity, float* deltaTimePtr);
	virtual ~Melee();
};