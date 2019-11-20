#include "ShootCar.h"

void ShootCar::followPath()
{
	if (pathSize >= 0 && (position - targetPos).Length() > 40)
	{
		destination = *path;
		if ((destination - position).Length() < 15)
		{
			path--;
			pathSize--;
		}
	}
	else
	{
		destination = targetPos;
	}
}

ShootCar::ShootCar()
{
}

ShootCar::ShootCar(float x, float z, Physics* physics)
	: Spitfire(x, z, physics)
{
	this->stats = VehicleStats::fastCar;
	setHealth(this->stats.maxHealth);
	attackRange = 10;
	this->weapon = WeaponHandler::getWeapon(WeaponType::aiFlamethrower);
}

ShootCar::~ShootCar()
{
}

void ShootCar::update(float dt, const Vector3& targetPos)
{
	Spitfire::update(dt, targetPos);
	updateBullets(deltaTime);
	if ((position - targetPos).Length() < attackRange)
	{
		shoot();
	}
}

Status ShootCar::shoot()
{
	float offset;
	Vector3 offsetPos;
	if ((position - targetPos).Length() < 23)
	{
		if (this->timeSinceLastShot >= this->weapon.fireRate)
		{
			this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);

			for (int i = 0; i < Ranged::bulletCount; i++)
			{
				if (bullets[i].getTimeLeft() == 0.0)
				{
					Vector3 dir = Vector3(vehicleBody1->getRigidBody()->getLinearVelocity());
					dir.Normalize();
					Vector3 bulletOrigin = position + dir;
					dir = (offsetPos - bulletOrigin);
					dir.Normalize();

					this->bullets[i].shoot(
						weapon,
						bulletOrigin,
						dir,
						Vector3(vehicleBody1->getRigidBody()->getLinearVelocity())*1.2,
						deltaTime
					);
					break;
				}
			}
		}
	}

	return Status::SUCCESS;
}
