#include "BossAbilities.h"
#include"../States/PlayingGameState.h"


BossAbilities::BossAbilities()
{
	Vector3 *vec = { 0 };
	this->positionPtr = vec;
	this->targetPosPtr = vec;
	this->velocityPtr = vec;
	this->flameThrowerDir = { 0, 0, 0 };
	this->flameXDir = 0;
	this->flameZDir = 0;
	WaitTimer waitTimer;

	this->timeSinceLastShot = 0;
	this->predicting = 0;
	this->weaponNr = 0;
	this->weapon = Weapon();

	this->waitTimer.wait_time = 2;
	this->waitTimer.time_left = 2;
	this->waitTimer.repeatable = true;

	this->attackRange = 0;
	this->dt = 0;
	this->frontVecShoot = { 0, 0, 0 };
}

BossAbilities::BossAbilities(Vector3* pos, Vector3* targetPos, Vector3* velocity, int weaponType)
{
	this->positionPtr = pos;
	this->targetPosPtr = targetPos;
	this->velocityPtr = velocity;
	this->flameThrowerDir = { 0, 0, 0 };
	this->flameXDir = 0;
	this->flameZDir = 0;
	this->frontVecShoot = { 0, 0, 0 };
	this->attackRange = 8;
	this->weaponNr = weaponType;
	assignWeapon(weaponType);

	this->waitTimer.wait_time = 2;
	this->waitTimer.time_left = 2;
	this->waitTimer.repeatable = 1;

	this->attackRange = 0;
	this->dt = 0;
	this->frontVecShoot = { 0, 0, 0 };
}

BossAbilities::~BossAbilities()
{

}

BossAbilities& BossAbilities::operator=(const BossAbilities& other)
{
	if (this != &other)
	{
		this->positionPtr = other.positionPtr;
		this->targetPosPtr = other.positionPtr;
		this->velocityPtr = other.velocityPtr;
		this->flameThrowerDir = other.flameThrowerDir;
		this->flameXDir = other.flameXDir;
		this->flameZDir = other.flameZDir;
		this->frontVecShoot = other.frontVecShoot;
		this->timeSinceLastShot = other.timeSinceLastShot;
		this->predicting = other.predicting;
		this->weaponNr = other.weaponNr;
		this->weapon = other.weapon;
		for (int i = 0; i < other.bulletCount; i++)
		{
			this->bullets[i] = other.bullets[i];
		}
		this->attackRange = other.attackRange;
		this->dt = other.dt;
		this->frontVecShoot = other.frontVecShoot;
	}
	return *this;
}

void BossAbilities::updateBullets(float deltaTime)
{
	this->timeSinceLastShot += deltaTime;
	for (int i = 0; i < this->bulletCount; i++)
	{
		bullets[i].update(deltaTime);
	}
}

Status BossAbilities::inAttackRange()
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

void BossAbilities::assignWeapon(int weaponType)
{
	if (weaponType == 1) // MachineGun
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiBossMachineGun);
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
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiBossFlamethrower);
		//	this->setColor(Vector4(0.0f, 1.0f, 1.0f, 1.0f));
	}
}

Status BossAbilities::shoot()
{
	if (this->weaponNr == 1) //machinegun
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

				for (int i = 0; i < BossAbilities::bulletCount; i++)
				{
					if (bullets[i].getTimeLeft() == 0.0)
					{
						Vector3 dir = (*targetPosPtr - *this->positionPtr);
						dir.Normalize();
						Vector3 bulletOrigin = *this->positionPtr + dir;
						dir = (offsetPos - bulletOrigin);
						dir.Normalize();

						this->bullets[i].shoot(
							weapon,
							bulletOrigin,
							dir,
							*velocityPtr,
							dt
						);
						break;
					}
				}
			}
		}
	}
	else if (this->weaponNr == 4) //flamethrower
	{
		//here make direction for shoot
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

				for (int i = 0; i < BossAbilities::bulletCount; i++)
				{
					if (bullets[i].getTimeLeft() == 0.0)
					{
						float flameXDirRad = ((this->flameXDir * M_PI) / 180); //degree to radians
						float flameZDirRad = ((this->flameZDir * M_PI) / 180);

						float xCos = cos(flameXDirRad);
						float zSin = sin(flameZDirRad);

						std::cout << xCos;
						CONTINUE HERER WITH FLAME THREOWER 360 DEGREEs

						this->flameThrowerDir.x += xCos;
						this->flameThrowerDir.z += zSin;

						this->flameXDir += 1;
						this->flameZDir += 1;

						if (this->flameXDir == 360)
							int stop = 1;


						Vector3 dir = (this->flameThrowerDir); //from boss to dir
						dir.Normalize();

						dir.Normalize();
						Vector3 bulletOrigin = *this->positionPtr + (dir * 3);
						/*dir = (offsetPos - bulletOrigin);
						dir.Normalize();*/

						this->bullets[i].shoot(
							weapon,
							bulletOrigin,
							dir,
							*velocityPtr,
							dt
						);
						break;
					}
				}
			}
		}
	}

	return Status::SUCCESS;
}

Status BossAbilities::ability1() //not done
{
	return Status::SUCCESS;
}

Status BossAbilities::switchWeapon()
{
	//Fix the length of player to boss
	float length = sqrt(pow(this->targetToSelf.x, 2) + pow(this->targetToSelf.y, 2) + pow(this->targetToSelf.z, 2));

	if (length <= 15.0f)
	{
		this->weaponNr = 4;
		assignWeapon(this->weaponNr);
	}
	else
	{
		this->weaponNr = 1;
		assignWeapon(this->weaponNr);
	}


	//assignWeapon(this->weaponNr);
	//this->weaponNr++;

	//if (this->weaponNr == 5)
	//	this->weaponNr = 1;

	return Status::SUCCESS;
}

Status BossAbilities::waitForStart()
{
	std::cout << "Patrol wait time : " << waitTimer.wait_time << std::endl;
	std::cout << "Patrol time left on patrol : " << waitTimer.time_left << std::endl;
	Status status = Status::WAIT;
	if (waitTimer.time_left <= 0.0f)
	{
		std::cout << "Patrol continue : " << waitTimer.time_left << std::endl;
		//switchWeapon();
		status = Status::SUCCESS;
		waitTimer.time_left = 0;
		if (waitTimer.repeatable)
		{
			waitTimer.time_left = waitTimer.wait_time;
		}
	}
	waitTimer.time_left -= this->dt;
	return status;
}