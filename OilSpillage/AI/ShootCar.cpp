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

ShootCar::ShootCar(float x, float z, int weaponType, Physics* physics)
	: Spitfire(x, z, physics), Ranged(&this->position, &this->targetPos, &this->velocity, &this->deltaTime, weaponType)
{
	this->stats = VehicleStats::fastCar;
	setHealth(this->stats.maxHealth);
	attackRange = 15;
	setUpActor();
	createRigidbody(physics);
	scaling(weapon.damage, 1.1);
}
void ShootCar::createRigidbody(Physics* physics)
{
	btRigidBody* tempo = physics->addBox(
		btVector3(getPosition().x, getPosition().y, getPosition().z),
		btVector3(getScale().x, getScale().y, getScale().z),
		10.0f,
		this);
	setRigidBody(tempo, physics);
	getRigidBody()->activate();
	getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	getRigidBody()->setFriction(0);
	getRigidBody()->setLinearFactor(btVector3(1, 0, 1));
	AABB vehicleBodyAABB = vehicleBody1->getAABB();
	btVector3 origin = btVector3(getPosition().x, getPosition().y + 0.65f, getPosition().z);
	btVector3 size = btVector3(vehicleBodyAABB.maxPos.x - vehicleBodyAABB.minPos.x,
		(vehicleBodyAABB.maxPos.y - vehicleBodyAABB.minPos.y) * 0.2f,
		vehicleBodyAABB.maxPos.z - vehicleBodyAABB.minPos.z) * 0.5f;
	tempo = physics->addBox(origin, size, 1.0f, this);
	vehicleBody1->setRigidBody(tempo, physics);
	vehicleBody1->getRigidBody()->activate();
	vehicleBody1->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	vehicleBody1->getRigidBody()->setFriction(1);
	pointJoint = physics->addPointJoint(getRigidBody(), this->vehicleBody1->getRigidBody());
}
void ShootCar::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&ShootCar::inAttackRange, std::ref(*this)));
	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&ShootCar::shoot, std::ref(*this)));

	root->addChildren(sequence);
	sequence.addChildren(inRange);
	sequence.addChildren(shoot);

}
ShootCar::~ShootCar()
{
}

void ShootCar::update(float dt, const Vector3& targetPos)
{
	Spitfire::update(dt, targetPos);
	updateBullets(deltaTime);
}

Status ShootCar::shoot()
{
	if (this->timeSinceLastShot >= this->weapon.fireRate)
	{
		this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);

		for (int i = 0; i < Ranged::bulletCount; i++)
		{
			if (bullets[i].getTimeLeft() == 0.0)
			{
				if (weapon.type == WeaponType::aiFlamethrower)
				{
					Vector3 dir = Vector3(vehicleBody1->getRigidBody()->getLinearVelocity());
					dir.Normalize();
					Vector3 bulletOrigin = position + dir;

					this->bullets[i].shoot(
						weapon,
						bulletOrigin,
						dir,
						Vector3(vehicleBody1->getRigidBody()->getLinearVelocity()) * 1.2,
						deltaTime
					);
				}
				else if (weapon.type == WeaponType::aiLaser)
				{
					Vector3 dir = (targetPos - position);
					dir.Normalize();
					Vector3 bulletOrigin = position + dir;

					this->bullets[i].shoot(
						weapon,
						bulletOrigin,
						dir,
						Vector3(0.0f),
						deltaTime
					);
				}
				else
				{
					Vector3 dir = (targetPos - position);
					dir.Normalize();
					Vector3 bulletOrigin = position + dir;

					this->bullets[i].shoot(
						weapon,
						bulletOrigin,
						dir,
						Vector3(vehicleBody1->getRigidBody()->getLinearVelocity()),
						deltaTime
					);
				}
				break;
			}
		}
	}

	return Status::SUCCESS;
}
