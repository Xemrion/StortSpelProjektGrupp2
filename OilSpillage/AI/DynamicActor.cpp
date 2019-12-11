#include "DynamicActor.h"

DynamicActor::DynamicActor()
{
}

DynamicActor::DynamicActor(float x, float z, Physics* physics)
	:Actor(x, z, physics)
{
	this->pathSize = -1;
}

DynamicActor::~DynamicActor()
{
}

void DynamicActor::move()
{
	Vector3 newVelocity = calculateVelocity();
	if (newVelocity != Vector3())
	{
		velocity = newVelocity;
		velocity.Normalize();
	}
	//If newVelocity is 0, slow down the velocity instead
	else
	{
		velocity.x -= deltaTime;
		velocity.z -= deltaTime;
		velocity.x = max(velocity.x, 0);
		velocity.z = max(velocity.z, 0);
	}
	Vector3 nextPos = position + Vector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime) * stats.speed;
	this->getRigidBody()->setLinearVelocity(btVector3(velocity.x, 0.0f, velocity.z) * (stats.speed) * 1.5);
	Vector3 targetToSelf = (nextPos - position);

	//Rotate
	if ( ((targetToSelf.x * vecForward.x) + (targetToSelf.z * vecForward.z)) < 0.8)
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
	if(stunnedTimer <= 0)
	{
		Actor::update(dt, targetPos);
		followPath();
		move();
	}
	stunnedTimer -= dt;
	onFire();
}

Vector3 DynamicActor::calculateVelocity()
{
	return destination - position;
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

void DynamicActor::knockBack(Vector3 direction, float force)
{
	direction.Normalize();
	this->getRigidBody()->setLinearVelocity(-btVector3(direction.x, 0.0f, direction.z) * force);
}

void DynamicActor::setGroup(void* newGroup)
{
	this->curGroup = newGroup;
}

void* DynamicActor::getGroup() const
{
	return this->curGroup;
}

void DynamicActor::setStun(float timer)
{
	stunnedTimer = timer;
	getRigidBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
}
