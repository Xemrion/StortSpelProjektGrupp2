#include "DynamicActor.h"

DynamicActor::DynamicActor()
{
}

DynamicActor::DynamicActor(float x, float z, Physics* physics)
	:Actor(x, z,physics)
{
	this->maxSpeed = 3.5f;
	this->maxForce = 0.5f;
	this->acceleration = Vector3(0.0f);
	this->boidOffset = 9;
	this->path = nullptr;
	this->state = State::Idle;
	this->aggroRange = 80;
	btRigidBody* tempo = physics->addSphere(1.0f, btVector3(x, position.y, z), 0.5f, this);
	setRigidBody(tempo, physics);
	getRigidBody()->activate();
	getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	getRigidBody()->setFriction(0);
	getRigidBody()->setLinearFactor(btVector3(1, 0, 1));


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
	this->getRigidBody()->setLinearVelocity(btVector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime) * (stats.maxSpeed * 100)*0.9);
	Vector3 targetToSelf = (temp - position);
	//Rotate
	if ((targetToSelf).Dot(vecForward) < 0.8)
	{
		vecForward -= (targetToSelf * deltaTime) / 0.02f;
		vecForward.Normalize();

		float newRot = atan2(this->vecForward.x, this->vecForward.z);

		this->setRotation(Vector3(0, newRot - (DirectX::XM_PI / 2), 0));
	}

	//position = temp;
	// Reset accelertion to 0 each cycle
	acceleration *= 0;
}

void DynamicActor::setPath(std::vector<Vector3>* path)
{
	this->path = path;

}

void DynamicActor::update(float dt, const Vector3& targetPos)
{
	Actor::update(dt, targetPos);
	if (this->state != State::Idle)
	{
		followPath();

	}
	else
	{
		destination = position;
	}
	move();

	if (this->state == State::Circulate)
	{
		this->circulatePlayer();
	}
}

void DynamicActor::applyForce(const Vector3& force)
{
	acceleration += force;
}

int DynamicActor::getBoidOffset()const
{
	return this->boidOffset;
}

float DynamicActor::getMaxSpeed()const
{
	return this->maxSpeed;
}

float DynamicActor::getMaxForce()const
{
	return this->maxForce;
}

const Vector3& DynamicActor::getVelocity()const
{
	return this->velocity;
}

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

void DynamicActor::moveCirculate(const Vector3& desiredDirection)
{
	//To make the slow down not as abrupt
	acceleration *= 0.4f;
	// Update velocity
	//velocity += acceleration;
	velocity = seekCirculate(desiredDirection);
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

Vector3 DynamicActor::seekCirculate(const Vector3& desiredDirection)
{
	acceleration = desiredDirection; //-velocity
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
		if ((position - targetPos).Length() < 15)
		{
			destination = targetPos;
		}
			else if (path->size() > 0)
			{
				destination = path->at(path->size() - 1);
				if((path->at(path->size() -1) - position).Length() < 2)
				{
					path->pop_back();
				}
			}
	}
}

void DynamicActor::circulatePlayer()
{
	std::vector<Vector3> pointsAroundPlayer;
	int currentPoint = 0;
	Vector3 playerToAI = position - destination;
	Vector3 radiusFromPlayer = { 10, 0, 10 };

	pointsAroundPlayer.push_back({ playerToAI.x, playerToAI.y, playerToAI.z + radiusFromPlayer.z });
	pointsAroundPlayer.push_back({ playerToAI.x + sqrt(radiusFromPlayer.x), playerToAI.y, playerToAI.z + sqrt(radiusFromPlayer.z) });
	pointsAroundPlayer.push_back({ playerToAI.x + radiusFromPlayer.x, playerToAI.y, playerToAI.z });
	pointsAroundPlayer.push_back({ playerToAI.x + sqrt(radiusFromPlayer.x), playerToAI.y, playerToAI.z - sqrt(radiusFromPlayer.z) });
	pointsAroundPlayer.push_back({ playerToAI.x, playerToAI.y, playerToAI.z - radiusFromPlayer.z });
	pointsAroundPlayer.push_back({ playerToAI.x - sqrt(radiusFromPlayer.x), playerToAI.y, playerToAI.z - sqrt(radiusFromPlayer.z) });
	pointsAroundPlayer.push_back({ playerToAI.x - radiusFromPlayer.x, playerToAI.y, playerToAI.z });
	pointsAroundPlayer.push_back({ playerToAI.x - sqrt(radiusFromPlayer.x), playerToAI.y, playerToAI.z + sqrt(radiusFromPlayer.z) });

	moveCirculate(pointsAroundPlayer[currentPoint]);
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

Status DynamicActor::setCirculateState()
{
	this->state = State::Circulate;
	return Status::SUCCESS;
}
