#include "Ranged.h"
#include"../States/PlayingGameState.h"
#include "../Lights.h"

void Ranged::updateBullets(float deltaTime)
{
	this->timeSinceLastShot += deltaTime;
	for (int i = 0; i < Ranged::bulletCount; i++)
	{
		if (bullets[i].getWeaponType() == WeaponType::aiLaser)
		{
			bullets[i].getGameObject()->setPosition(*positionPtr);
			const float laserDropOffSpeed = 1.0;
			laser->setDirection(bullets[i].getDirection());
			laser->setPos(bullets[i].getGameObject()->getPosition());
			float dropOffFactor = weapon.bulletLifetime - bullets[i].getTimeLeft();
			laser->setLuminance(max(0.2 - (dropOffFactor * laserDropOffSpeed) * laserDropOffSpeed, 0.0));
		}
		bullets[i].update(deltaTime);
	}
}
Status Ranged::inAttackRange()
{
	Status status;

	if ((*positionPtr - *targetPosPtr).Length() > attackRange)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

void Ranged::assignWeapon(int weaponType)
{
	if (Game::getGameInfo().nrOfClearedStages < 3)
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiMachineGun);
	}
	else
	{
		if (weaponType <= 4)
		{
			this->weapon = WeaponHandler::getWeapon(WeaponType::aiMachineGun);
			this->predicting = true;
		}
		else if (weaponType <= 6)
		{
			this->weapon = WeaponHandler::getWeapon(WeaponType::aiLaser);
			LaserLight tempLaser = LaserLight(
				Vector3(0, 0, 0),
				Vector3(1.0, 0.1, 0.1),
				0.0,
				Vector3(0, 0, 0),
				weapon.bulletScale.z);
			PlayingGameState* gameState = static_cast<PlayingGameState*>(Game::getCurrentState());
			laser = gameState->addLight(tempLaser);
		}
		else if (weaponType <= 8)
		{
			this->weapon = WeaponHandler::getWeapon(WeaponType::aiFlamethrower);
		}
	}

}
Status Ranged::shoot()
{
	float offset;
	Vector3 offsetPos;
	if (!predicting)
	{
		offset = (rand() % 4) - 2;
		offset *= 0.9f;
		offsetPos = Vector3(targetPosPtr->x - offset, 0.0f, targetPosPtr->z - offset);
	}
	else
	{
		Vector3 targetVelocity = Vector3(static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getRigidBody()->getLinearVelocity());

		float predictionFactor = 0.4f;
		offsetPos = Vector3(targetPosPtr->x + targetVelocity.x * predictionFactor, 0.0f, (targetPosPtr->z + targetVelocity.z * predictionFactor));
	}
	if ((*positionPtr - *targetPosPtr).Length() < 23)
	{
		if (this->timeSinceLastShot >= this->weapon.fireRate)
		{
			this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);

			for (int i = 0; i < Ranged::bulletCount; i++)
			{
				if (bullets[i].getTimeLeft() == 0.0)
				{
					if (weapon.type != WeaponType::aiLaser)
					{
						Vector3 dir = (offsetPos - *this->positionPtr);
						dir.Normalize();
						Vector3 bulletOrigin = *this->positionPtr + dir;

						this->bullets[i].shoot(
							weapon,
							bulletOrigin,
							dir,
							*velocityPtr,
							*deltaTimePtr
						);
					}
					else
					{
						Vector3 dir = (*targetPosPtr - *this->positionPtr);
						dir.Normalize();
						Vector3 bulletOrigin = *this->positionPtr + dir;

						this->bullets[i].shoot(
							weapon,
							bulletOrigin,
							dir,
							Vector3(),
							*deltaTimePtr
						);
					}

					break;
				}
			}
		}
	}

	return Status::SUCCESS;
}
Ranged::Ranged()
{

}

Ranged::Ranged(Vector3* pos, Vector3* targetPos, Vector3* velocity, float* deltaTimePtr, int weaponType)
{
	this->positionPtr = pos;
	this->targetPosPtr = targetPos;
	this->velocityPtr = velocity;
	this->attackRange = 8;
	this->deltaTimePtr = deltaTimePtr;
	assignWeapon(weaponType);
}

Ranged::~Ranged()
{
	if (laser != nullptr)
	{
		PlayingGameState* gameState = dynamic_cast<PlayingGameState*>(Game::getCurrentState());
		if (gameState != nullptr)
		{
			gameState->removeLight(laser);
		}
	}
}
