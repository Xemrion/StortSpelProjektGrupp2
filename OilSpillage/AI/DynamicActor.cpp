#include "DynamicActor.h"

DynamicActor::DynamicActor()
{
}

DynamicActor::DynamicActor(float x, float z, Physics* physics)
	:Actor(x, z, physics)
{
	this->pathSize = -1;
	this->aggroRange = 1000;
}

DynamicActor::~DynamicActor()
{

}

void DynamicActor::move()
{
	velocity = calculateVelocity();
	velocity.Normalize();
	Vector3 nextPos = position + Vector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime) * stats.speed;
	this->getRigidBody()->setLinearVelocity(btVector3(velocity.x, 0.0f, velocity.z) * (stats.speed) * 1.5);
	Vector3 targetToSelf = (nextPos - position);

	//Rotate
	if ((targetToSelf).Dot(vecForward) < 0.8)
	{
		vecForward -= (targetToSelf * deltaTime) / 0.02f;
		vecForward.Normalize();

		float newRot = atan2(this->vecForward.x, this->vecForward.z);

		this->setRotation(Vector3(0, newRot - (DirectX::XM_PI / 2), 0));
	}

}

void DynamicActor::setPath(Vector3* path)
{
	this->path = path;
}

void DynamicActor::update(float dt, const Vector3& targetPos)
{
	Actor::update(dt, targetPos);

	followPath();
	move()
	onFire();
}

Vector3 DynamicActor::calculateVelocity()
{
	Vector3 desiredDirection = destination - position;
	return desiredDirection - velocity;
}

void DynamicActor::followPath()
{
	if ((position - targetPos).Length() < 15)
	{
		destination = targetPos;
	}
	else if (pathSize >= 0)
	{
		destination = *path;
		if ((destination - position).Length() < 2)
		{
			path--;
			pathSize--;
		}
	}
}

Status DynamicActor::setChaseState()
{
	return Status::SUCCESS;
}

Status DynamicActor::inAggroRange()
{
	Status status;

	if ((getPosition() - targetPos).Length() > aggroRange)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

Status DynamicActor::setIdleState()
{
	return Status::SUCCESS;
}
