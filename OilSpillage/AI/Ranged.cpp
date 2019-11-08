#include "Ranged.h"
#include"../States/PlayingGameState.h"
void Ranged::updateBullets(float deltaTime)
{
	this->timeSinceLastShot += deltaTime;
	for (int i = 0; i < Ranged::bulletCount; i++)
	{
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
	if (weaponType == 1) // MachineGun
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiMachineGun);
		//this->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else if (weaponType == 2)// MissileLauncher
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiMissileLauncher);
		//this->setColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
		this->predicting = true;
	}
	else if (weaponType == 3)// Laser
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiLaser);
		//this->setColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	}
	else if (weaponType == 4)// Flamethrower
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiFlamethrower);
	//	this->setColor(Vector4(0.0f, 1.0f, 1.0f, 1.0f));
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
		Vector3 targetVelocity = Vector3(static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getVehicle()->getRigidBody()->getLinearVelocity());
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
					Vector3 dir = (*targetPosPtr - *this->positionPtr);
					dir.Normalize();
					Vector3 bulletOrigin = *this->positionPtr + dir;
					dir = (offsetPos - bulletOrigin);
					dir.Normalize();

					this->bullets[i].setWeaponType(this->weapon.type);
					this->bullets[i].shoot(
						weapon,
						bulletOrigin,
						dir,
						*velocityPtr
					);
					break;
				}
			}
		}
	}

	return Status::SUCCESS;
}
Ranged::Ranged()
{
	this->timeSinceLastShot = 0;
	//Assign weapon()
}

Ranged::Ranged(Vector3* pos, Vector3* targetPos, Vector3* velocity, int weaponType)
{
	this->positionPtr = pos;
	this->targetPosPtr = targetPos;
	this->velocityPtr = velocity;
	assignWeapon(weaponType);
}