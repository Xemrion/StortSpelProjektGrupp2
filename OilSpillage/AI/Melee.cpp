#include "Melee.h"
#include"../States/PlayingGameState.h"
void Melee::meleeAttack()
{
	if (curCooldown <= 0)
	{
		bullet.shoot(weapon, *positionPtr, *velocityPtr, Vector3(), *deltaTimePtr);
		bullet.update(*deltaTimePtr);
		curCooldown = attackCooldown;
		for (int i = 0; i < 30; i++)
		{
			Game::getGraphics().addParticle(*positionPtr, *velocityPtr, 15.0f, 0.75f);
		}
	}
	curCooldown -= *deltaTimePtr;
}

Melee::Melee()
{
}

Melee::Melee(Vector3* pos, Vector3* velocity, float* deltaTimePtr)
{
	this->positionPtr = pos;
	this->deltaTimePtr = deltaTimePtr;
	this->velocityPtr = velocity;
	this->weapon = WeaponHandler::getWeapon(WeaponType::aiMelee);
	curCooldown = attackCooldown;
}

Melee::~Melee()
{
}