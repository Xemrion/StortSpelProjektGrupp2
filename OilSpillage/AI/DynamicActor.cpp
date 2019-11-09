#include "DynamicActor.h"

DynamicActor::DynamicActor()
{
}

DynamicActor::DynamicActor(float x, float z)
	:Actor(x, z)
{
	this->maxSpeed = 3.5f;
	this->maxForce = 0.5f;
	this->acceleration = Vector3(0.0f);
	this->boidOffset = 9;
	this->path = nullptr;
	this->state = State::Idle;
	this->aggroRange = 20;

}

DynamicActor::~DynamicActor()
{
}

void DynamicActor::move()
{
	//To make the slow down not as abrupt
	acceleration *= 0.4f;
	// Update velocity
	velocity += acceleration;
	velocity += seek();
	// Limit speed
	if (velocity.Length() > maxForce)
	{
		velocity /= velocity.Length();
	}

	Vector3 temp = position + Vector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime) * stats.maxSpeed;
	Vector3 targetToSelf = (temp - position);
	//Rotate
	if ((targetToSelf).Dot(vecForward) < 0.8)
	{
		vecForward -= (targetToSelf * deltaTime) / 0.02f;
		vecForward.Normalize();

		float newRot = atan2(this->vecForward.x, this->vecForward.z);
		
		this->setRotation(Vector3(0, newRot - (DirectX::XM_PI / 2), 0));
	}

	position = temp;
	// Reset accelertion to 0 each cycle
	acceleration *= 0;
}

void DynamicActor::setPath(std::vector<Vector3>* path)
{
	this->path = path;

}

void DynamicActor::update(float dt, Vector3 targetPos)
{
	Actor::update(dt, targetPos);
	if (this->state != State::Idle)
	{
		followPath();
		move();
	}
	else
	{
		//idle
	}
}

void DynamicActor::applyForce(Vector3 force)
{
	acceleration += force;
}

//Vector3 Dynamic::separation(std::vector<Actor*>& boids, std::vector<Vector3> buildings, Vector3 targetPos)
//{
//	// Distance of field of vision for separation between boids
//	float desiredSeparationDistance = boidOffset;
//	Vector3 direction(0.0f);
//	float nrInProximity = 0.0f;
//	int size = boids.size();
//	// For every boid in the system, check if it's too close
//	for (int i = 0; i < size; i++)
//	{
//		// Calculate distance from current boid to boid we're looking at
//		Vector3 curBoidPos = boids[i]->getPosition();
//		float deltaX = position.x - curBoidPos.x;
//		float deltaZ = position.z - curBoidPos.z;
//		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
//		// If this is a fellow boid and it's too close, move away from it
//		if ((distance < desiredSeparationDistance) && distance != 0)
//		{
//			Vector3 difference(0.0f);
//			difference = position - boids.at(i)->getPosition();
//			difference.Normalize();
//			difference /= distance;      // Weight by distance
//			direction += difference;
//			nrInProximity++;
//		}
//	}
//	for (int i = 0; i < buildings.size(); i++)
//	{
//		// Calculate distance from current boid to boid we're looking at
//		Vector3 curBoidPos = buildings[i];
//		float deltaX = position.x - curBoidPos.x;
//		float deltaZ = position.z - curBoidPos.z;
//		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
//		// If this is a fellow boid and it's too close, move away from it
//		if ((distance < 110) && distance != 0)
//		{
//			Vector3 difference(0.0f);
//			difference = position - buildings[i];
//			difference.Normalize();
//			difference;      // Weight by distance
//			direction += difference;
//			nrInProximity++;
//		}
//	}
//	// Calculate distance from current boid to player
//	float deltaX = position.x - targetPos.x;
//	float deltaZ = position.z - targetPos.z;
//	float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
//	// If this is a fellow boid and it's too close, move away from it
//	if (distance < desiredSeparationDistance)
//	{
//		Vector3 difference(0.0f);
//		difference = position - targetPos;
//		difference.Normalize();
//		if (distance != 0)
//		{
//			difference /= distance;      // Weight by distance
//		}
//		direction += difference;
//		nrInProximity++;
//	}
//	// Adds average difference of location to acceleration
//	if (nrInProximity > 0)
//	{
//		direction /= nrInProximity;
//	}
//	if (direction.Length() > 0.0f)
//	{
//		// Steering = Desired - Velocity
//		direction.Normalize();
//		direction *= maxSpeed;
//		direction -= velocity;
//		if (direction.Length() > maxForce)
//		{
//			direction /= direction.Length();
//		}
//
//	}
//	return direction;
//}

Vector3 DynamicActor::seek()
{
	Vector3 desiredDirection;
	desiredDirection -= position - destination;
	//desired *= maxSpeed;

	acceleration = desiredDirection - velocity;
	if (acceleration.Length() > maxForce)
	{
		acceleration /= acceleration.Length();
	}
	return acceleration;
}

void DynamicActor::followPath()
{
	if (path != nullptr)
	{
		if (path->size() > 0)
		{
			destination = path->at(path->size() - 1);
			if (position.Distance(path->at(path->size() - 1), position) < 2)
			{
				path->pop_back();
			}
		}
		else
		{
			destination = targetPos;
		}
	}
}

Status DynamicActor::setChaseState()
{
	this->state = State::Chasing;
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
	this->state = State::Idle;
	return Status::SUCCESS;
}
