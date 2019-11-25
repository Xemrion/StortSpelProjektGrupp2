#include "ChaseCar.h"

void ChaseCar::createRigidbody(Physics* physics)
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

void ChaseCar::followPath()
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

ChaseCar::ChaseCar()
{
}

ChaseCar::ChaseCar(float x, float z,Physics* physics)
	:Spitfire(x, z,physics)
{
	this->stats = VehicleStats::fastCar;
	setHealth(this->stats.maxHealth);
	createRigidbody(physics);
}

ChaseCar::~ChaseCar()
{
}

void ChaseCar::update(float dt, const Vector3& targetPos)
{
	Spitfire::update(dt, targetPos);
}
