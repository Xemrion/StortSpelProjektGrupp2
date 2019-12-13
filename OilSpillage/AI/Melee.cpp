#include "Melee.h"
#include"../States/PlayingGameState.h"
void Melee::meleeAttack()
{
	if (curCooldown <= 0)
	{
		Vector3 position = GameObjectBase::btVectorConv(positionPtr->getWorldTransform().getOrigin());
		velocityPtr->Normalize();
		bullet.shoot(weapon, position, *velocityPtr, Vector3(), *deltaTimePtr);
		bullet.update(*deltaTimePtr);
		curCooldown = attackCooldown;
		for (int i = 0; i < 30; i++)
		{
			Game::getGraphics().addParticle(position, *velocityPtr, 15.0f, 0.75f);
		}
	}
	curCooldown -= *deltaTimePtr;
}

Melee::Melee()
{
}

Melee::Melee(btRigidBody* pos, Vector3* velocity, float* deltaTimePtr)
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