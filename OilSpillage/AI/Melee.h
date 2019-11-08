#pragma once
#include "..//Weapon.h"

class Melee
{
protected:
	virtual void assignWeapon(int weaponType);
	Weapon weapon;
public:
	Melee();
	Melee(int weaponType);
	virtual ~Melee();
};