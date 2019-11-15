#include "Melee.h"

void Melee::meleeAttack()
{
	bullet.shoot(weapon, *positionPtr, *velocityPtr, Vector3(), *deltaTimePtr);
	bullet.update(*deltaTimePtr);
}

Melee::Melee()
{
}

Melee::Melee(Vector3* pos,Vector3* velocity, float* deltaTimePtr)
{
	this->positionPtr = pos;
	this->deltaTimePtr = deltaTimePtr;
	this->velocityPtr = velocity;

	this->weapon = WeaponHandler::getWeapon(WeaponType::aiMelee);
}

Melee::~Melee()
{
}
