#include "BossAbilities.h"
#include"../States/PlayingGameState.h"

BossAbilities::BossAbilities()
{
	Vector3 *vec = { 0 };
	this->positionPtr = vec;
	this->targetPosPtr = vec;
	this->velocityPtr = vec;
	this->flameXDirDegree = 0;
	this->flameZDirDegree = 0;
	this->turnToShoot = 0;
	this->turnToShoot2 = 0;
	WaitTimer waitTimer;

	this->timeSinceLastShot = 0;
	this->weaponSwitchTimer = 0;

	this->predicting = 0;
	this->weaponNr = 0;
	this->weapon = Weapon();

	this->waitTimer.wait_time = 2;
	this->waitTimer.time_left = 2;
	this->waitTimer.repeatable = true;
	this->dt = 0;
	this->frontVecShoot = { 0, 0, 0 };
}

BossAbilities::BossAbilities(Vector3* pos, Vector3* targetPos, Vector3* velocity, int weaponType)
{
	this->positionPtr = pos;
	this->targetPosPtr = targetPos;
	this->velocityPtr = velocity;
	this->flameXDirDegree = 0;
	this->flameZDirDegree = 0;
	this->turnToShoot = 0;
	this->turnToShoot2 = 0;

	this->timeSinceLastShot = 0;
	this->weaponSwitchTimer = 0;

	this->frontVecShoot = { 0, 0, 0 };
	this->weaponNr = weaponType;
	assignWeapon(weaponType);

	this->waitTimer.wait_time = 2;
	this->waitTimer.time_left = 2;
	this->waitTimer.repeatable = 1;
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
		this->flameXDirDegree = other.flameXDirDegree;
		this->flameZDirDegree = other.flameZDirDegree;
		this->turnToShoot = other.turnToShoot;
		this->turnToShoot2 = other.turnToShoot2;
		this->frontVecShoot = other.frontVecShoot;
		this->timeSinceLastShot = other.timeSinceLastShot;
		this->weaponSwitchTimer = other.weaponSwitchTimer;
		this->predicting = other.predicting;
		this->weaponNr = other.weaponNr;
		this->weapon = other.weapon;
		for (int i = 0; i < other.bulletCount; i++)
		{
			this->bullets[i] = other.bullets[i];;
		}
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

	this->weaponSwitchTimer += deltaTime;
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
	else if (weaponType == 2)// Flamethrower
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiBossFlamethrower);
		//	this->setColor(Vector4(0.0f, 1.0f, 1.0f, 1.0f));
	}
	else if (weaponType == 3)// MissileLauncher
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiBossMissileLauncher);
		//this->setColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
		this->predicting = true;
	}
	else if (weaponType == 4)
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiBossFlamethrowerPhase2);
		this->predicting = true;
	}
	else if (weaponType == 5)
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiBossMachineGunPhase2);
		//this->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		this->predicting = true;
	}
	
}

Status BossAbilities::shoot()
{
	if (phaseShoot == 1)
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
				Vector3 targetVelocity = Vector3(static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getRigidBody()->getLinearVelocity());

				float predictionFactor = 0.4f;
				offsetPos = Vector3(targetPosPtr->x + targetVelocity.x * predictionFactor, 0.0f, (targetPosPtr->z + targetVelocity.z * predictionFactor));
			}
			if ((*positionPtr - *targetPosPtr).Length() < 9999) // range, depends on change weapon func
			{
				if (this->timeSinceLastShot >= this->weapon.fireRate)
				{
					this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);

					for (int i = 0; i < BossAbilities::bulletCount; i++)
					{
						if (bullets[i].getTimeLeft() == 0.0)
						{
							Vector3 dir = (*targetPosPtr - *this->positionPtr);
							Vector3 dir2 = (*targetPosPtr - *this->positionPtr);
							dir.Normalize();
							dir2.Normalize();
							Vector3 bulletOrigin = *this->positionPtr + (this->rightVecShoot * 2.5) + (this->frontVecShoot * -3);
							Vector3 bulletOriginLeft = *this->positionPtr + (this->rightVecShoot * 2.5 * -1) + (this->frontVecShoot * -3);
							bulletOrigin.y = 0.5;
							bulletOriginLeft.y = 0.5;
							dir = (offsetPos - bulletOrigin);
							dir2 = (offsetPos - bulletOriginLeft);
							dir.Normalize();
							dir2.Normalize();

							if (this->turnToShoot2 % 2 == 0)
							{
								this->bullets[i].shoot(
									weapon,
									bulletOrigin,
									dir,
									*velocityPtr,
									dt);
							}
							if (this->turnToShoot2 % 2 == 1)
							{
								this->bullets[i].shoot(
									weapon,
									bulletOriginLeft,
									dir2,
									*velocityPtr,
									dt);
							}
							break;
						}
						this->turnToShoot2++;
					}
					this->turnToShoot2 = 0;
				}
			}
		}
		else if (this->weaponNr == 2) //flamethrower
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
				Vector3 targetVelocity = Vector3(static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getRigidBody()->getLinearVelocity());

				float predictionFactor = 0.4f;
				offsetPos = Vector3(targetPosPtr->x + targetVelocity.x * predictionFactor, 0.0f, (targetPosPtr->z + targetVelocity.z * predictionFactor));
			}
			if ((*positionPtr - *targetPosPtr).Length() < 9999)
			{
				if (this->timeSinceLastShot >= this->weapon.fireRate)
				{
					this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);

					for (int i = 0; i < BossAbilities::bulletCount; i++)
					{
						if (bullets[i].getTimeLeft() == 0.0)
						{
							float flameXDirRad = ((this->flameXDirDegree * M_PI) / 180); //degree to radians
							float flameZDirRad = ((this->flameZDirDegree * M_PI) / 180);

							float xCos = cos(flameXDirRad);
							float zSin = sin(flameZDirRad);

							Vector3 flameThrowerDir;

							flameThrowerDir.x = xCos;
							flameThrowerDir.z = zSin;

							this->flameXDirDegree += this->dt * 250;
							this->flameZDirDegree += this->dt * 250;


							Vector3 dir = (flameThrowerDir); //from boss to dir
							dir.Normalize();

							Vector3 bulletOrigin = *this->positionPtr + (dir * 3);
							Vector3 bulletOriginNeg = *this->positionPtr + (-dir * 3);

							/*dir = (offsetPos - bulletOrigin);
							dir.Normalize();*/

							if (i % 2 == 0)
							{
								this->bullets[i].shoot(
									weapon,
									bulletOrigin,
									dir,
									*velocityPtr,
									dt);
							}
							else if (i % 2 == 1)
							{
								this->bullets[i].shoot(
									weapon,
									bulletOriginNeg,
									-dir,
									*velocityPtr,
									dt);
							}
							break;
						}
						this->turnToShoot++;
					}
					this->turnToShoot = 0;
				}
			}
		}
		else if (this->weaponNr == 3) //Missile
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
			if ((*positionPtr - *targetPosPtr).Length() < 9999) // range, depends on change weapon func
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
							bulletOrigin.y = 0.1;
							dir = (offsetPos - bulletOrigin);
							dir.Normalize();

							this->bullets[i].shoot(
								weapon,
								bulletOrigin,
								dir,
								*velocityPtr,
								dt);
						}
						break;
					}
					this->turnToShoot2++;
				}
				this->turnToShoot2 = 0;
			}
		}
	}
	else if (this->phaseShoot == 2)
	{
		if (this->weaponNr == 4)	//flamethrower phase2
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
				Vector3 targetVelocity = Vector3(static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getRigidBody()->getLinearVelocity());
	
				float predictionFactor = 0.4f;
				offsetPos = Vector3(targetPosPtr->x + targetVelocity.x * predictionFactor, 0.0f, (targetPosPtr->z + targetVelocity.z * predictionFactor));
			}
			if ((*positionPtr - *targetPosPtr).Length() < 9999)
			{
				if (this->timeSinceLastShot >= this->weapon.fireRate)
				{
					this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);
	
					for (int i = 0; i < BossAbilities::bulletCount; i++)
					{
						if (bullets[i].getTimeLeft() == 0.0)
						{
							float flameXDirRad = ((this->flameXDirDegree * M_PI) / 180); //degree to radians
							float flameZDirRad = ((this->flameZDirDegree * M_PI) / 180);
								
							float xCos = cos(flameXDirRad);
							float zSin = sin(flameZDirRad);
	
							Vector3 flameThrowerDir;
								
							flameThrowerDir.x = xCos;
							flameThrowerDir.z = zSin;
								
							this->flameXDirDegree += this->dt * 325;
							this->flameZDirDegree += this->dt * 325;
	
	
							Vector3 dir = (flameThrowerDir); //from boss to dir
							dir.Normalize();
								
							Vector3 bulletOrigin = *this->positionPtr + (dir * 3);
							Vector3 bulletOriginNeg = *this->positionPtr + (-dir * 3);
	
							/*dir = (offsetPos - bulletOrigin);
							dir.Normalize();*/
								
							if (i % 2 == 0)
							{
								this->bullets[i].shoot(
									weapon,
									bulletOrigin,
									dir,
									*velocityPtr,
									dt);
							}
							else if (i % 2 == 1)
							{
								this->bullets[i].shoot(
									weapon,
									bulletOriginNeg,
									-dir,
									*velocityPtr,
									dt);
							}
							break;
						}
						this->turnToShoot++;
					}
					this->turnToShoot = 0;
				}
			}
		}
		if (this->weaponNr == 5)
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
			if ((*positionPtr - *targetPosPtr).Length() < 9999) // range, depends on change weapon func
			{
				if (this->timeSinceLastShot >= this->weapon.fireRate)
				{
					this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);

					for (int i = 0; i < BossAbilities::bulletCount; i++)
					{
						if (bullets[i].getTimeLeft() == 0.0)
						{
							Vector3 dir = (*targetPosPtr - *this->positionPtr);
							Vector3 dir2 = (*targetPosPtr - *this->positionPtr);
							dir.Normalize();
							dir2.Normalize();
							Vector3 bulletOrigin = *this->positionPtr + (this->rightVecShoot * 2.5) + (this->frontVecShoot * -3);
							Vector3 bulletOriginLeft = *this->positionPtr + (this->rightVecShoot * 2.5 * -1) + (this->frontVecShoot * -3);
							bulletOrigin.y = 0.1;
							bulletOriginLeft.y = 0.1;
							dir = (offsetPos - bulletOrigin);
							dir2 = (offsetPos - bulletOriginLeft);
							dir.Normalize();
							dir2.Normalize();

							if (this->turnToShoot2 % 2 == 0)
							{
								this->bullets[i].shoot(
									weapon,
									bulletOrigin,
									dir,
									*velocityPtr,
									dt);
							}
							if (this->turnToShoot2 % 2 == 1)
							{
								this->bullets[i].shoot(
									weapon,
									bulletOriginLeft,
									dir2,
									*velocityPtr,
									dt);
							}
							break;
						}
						this->turnToShoot2++;
					}
					this->turnToShoot2 = 0;
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

	if (this->phaseShoot == 1)
	{
		if (length <= 18.5f) //flame
		{
			this->weaponNr = 2;
			assignWeapon(this->weaponNr);
		}
		else if (length > 18.5 && length <= 35) //machine
		{
			this->weaponNr = 1;
			assignWeapon(this->weaponNr);
		}
		else  //rocket
		{
			this->weaponNr = 3;
			assignWeapon(this->weaponNr);
		}
	}
	if (this->phaseShoot == 2)
	{
		if (length <= 22.5f)
		{
			this->weaponNr = 4;
			assignWeapon(this->weaponNr);
		}
		else
		{
			this->weaponNr = 5;
			assignWeapon(this->weaponNr);
		}
	}

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