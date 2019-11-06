#include "../States/PlayingGameState.h"
#include "Actor.h"

Actor::Actor()
{
	this->setUpActor();
	this->timeSinceLastShot = 0;

	this->acceleration = Vector3(0.0f);
	this->velocity = Vector3(10.0f, 0.0f, 10.0f);
	this->position = Vector3(0, 0.0f, 0);
	this->maxSpeed = 3.5f;
	this->maxForce = 0.5f;

	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
}

Actor::Actor(float x, float z, int weaponType)
{
	this->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(this);

	this->setUpActor();
	this->timeSinceLastShot = 0;
	this->acceleration = Vector3(0.0f);
	this->velocity = Vector3(10.0f, 0.0f, 10.0f);
	this->position = Vector3(x, -1.0f, z);
	this->maxSpeed = 3.5f;
	this->maxForce = 0.5f;
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
	assignWeapon(weaponType);
}

void Actor::assignWeapon(int weaponType)
{
	if (weaponType == 1) // MachineGun
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::aiMachineGun);
		this->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else if (weaponType == 2)// MissileLauncher
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::MissileLauncher);
		this->setColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
		this->predicting = true;
	}
	else if (weaponType == 3)// Laser
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::Laser);
		this->setColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f));
	}
	else if (weaponType == 4)// Flamethrower
	{
		this->weapon = WeaponHandler::getWeapon(WeaponType::Flamethrower);
		this->setColor(Vector4(0.0f, 1.0f, 1.0f, 1.0f));
	}
}

Actor::~Actor()
{
}

void Actor::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;
	this->root->func();
	followPath();
}

void Actor::updateWeapon(float deltaTime)
{
	this->timeSinceLastShot += deltaTime;
	for (int i = 0; i < Actor::bulletCount; i++)
	{
		bullets[i].update(deltaTime);
	}
}

Status Actor::shoot()
{
	float offset;
	Vector3 offsetPos;
	if (!predicting)
	{
		offset = (rand() % 4) - 2;
		offset *= 0.9f;
		offsetPos = Vector3(targetPos.x - offset, 0.0f, targetPos.z - offset);
	}
	else
	{
		Vector3 targetVelocity = Vector3(static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getVehicle()->getRigidBody()->getLinearVelocity());
		float predictionFactor = 0.4f;
		offsetPos = Vector3(targetPos.x + targetVelocity.x * predictionFactor, 0.0f, (targetPos.z + targetVelocity.z * predictionFactor));
	}

	if ((position - targetPos).Length() < 23)
	{
		if (this->timeSinceLastShot >= this->weapon.fireRate)
		{
			this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);

			for (int i = 0; i < Actor::bulletCount; i++)
			{
				if (bullets[i].getTimeLeft() == 0.0)
				{
					Vector3 dir = (targetPos - this->position);
					dir.Normalize();
					Vector3 bulletOrigin = this->position + dir;
					dir = (offsetPos - bulletOrigin);
					dir.Normalize();

					this->bullets[i].setWeaponType(this->weapon.type);
					this->bullets[i].shoot(
						weapon,
						bulletOrigin,
						dir,
						this->velocity
					);
					break;
				}
			}
		}
	}

	return Status::SUCCESS;
}

Status Actor::inAttackRange()
{
	Status status;

	if ((getPosition() - targetPos).Length() > attackRange)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

Status Actor::enemyNear()
{
	Status status;

	if ((getPosition() - targetPos).Length() > 20)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

Status Actor::setChaseState()
{
	this->state = State::Chasing;
	return Status::SUCCESS;
}

Status Actor::setRoamState()
{
	this->state = State::Roaming;
	return Status::SUCCESS;
}

void Actor::followPath()
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
		//else
		//{
		//	destination = targetPos;
		//}
	}

}

void Actor::applyForce(Vector3 force)
{
	acceleration += force;
}

Vector3 Actor::separation(vector<Actor*>& boids, Vector3 targetPos)
{
	// Distance of field of vision for separation between boids
	float desiredSeparationDistance = boidOffset;
	Vector3 direction(0.0f);
	float nrInProximity = 0.0f;
	int size = boids.size();
	// For every boid in the system, check if it's too close
	for (int i = 0; i < size; i++)
	{
		// Calculate distance from current boid to boid we're looking at
		Vector3 curBoidPos = boids[i]->position;
		float deltaX = position.x - curBoidPos.x;
		float deltaZ = position.z - curBoidPos.z;
		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		// If this is a fellow boid and it's too close, move away from it
		if ((distance < desiredSeparationDistance) && distance != 0)
		{
			Vector3 difference(0.0f);
			difference = position - boids.at(i)->position;
			difference.Normalize();
			difference /= distance;      // Weight by distance
			direction += difference;
			nrInProximity++;
		}
	}
	vector<Vector3> temp;
	temp = static_cast<PlayingGameState*>(Game::getCurrentState())->map->getTileMap().getAllTilePositionsInRadius(this->position, 120, Tile::building);
	for (int i = 0; i < temp.size(); i++)
	{
		// Calculate distance from current boid to boid we're looking at
		Vector3 curBoidPos = temp[i];
		float deltaX = position.x - curBoidPos.x;
		float deltaZ = position.z - curBoidPos.z;
		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		// If this is a fellow boid and it's too close, move away from it
		if ((distance < 110) && distance != 0)
		{
			Vector3 difference(0.0f);
			difference = position - temp[i];
			difference.Normalize();
			difference;      // Weight by distance
			direction += difference;
			nrInProximity++;
		}
	}
	// Calculate distance from current boid to player
	float deltaX = position.x - targetPos.x;
	float deltaZ = position.z - targetPos.z;
	float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
	// If this is a fellow boid and it's too close, move away from it
	if (distance < desiredSeparationDistance)
	{
		Vector3 difference(0.0f);
		difference = position - targetPos;
		difference.Normalize();
		if (distance != 0)
		{
			difference /= distance;      // Weight by distance
		}
		direction += difference;
		nrInProximity++;
	}
	// Adds average difference of location to acceleration
	if (nrInProximity > 0)
	{
		direction /= nrInProximity;
	}
	if (direction.Length() > 0.0f)
	{
		// Steering = Desired - Velocity
		direction.Normalize();
		direction *= maxSpeed;
		direction -= velocity;
		if (direction.Length() > maxForce)
		{
			direction /= direction.Length();
		}

	}
	return direction;
}

Vector3 Actor::seek()
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

void Actor::run(vector<Actor*>& boids, float deltaTime, Vector3 targetPos)
{
	applyForce(separation(boids, targetPos) * 4);
	update(deltaTime, targetPos);
	move(deltaTime);
}

void Actor::move(float deltaTime)
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

	Vector3 temp = position + Vector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime) * updatedStats.maxSpeed;
	Vector3 targetToSelf = (temp - position);
	//Rotate
	if ((targetToSelf).Dot(vecForward) < 0.8)
	{
		vecForward -= (targetToSelf * deltaTime) / 0.02f;
		vecForward.Normalize();

		float newRot = atan2(this->vecForward.x, this->vecForward.z);
		this->setRotation(Vector3(0, newRot - (XM_PI / 2), 0));
	}

	position = temp;
	// Reset accelertion to 0 each cycle
	acceleration *= 0;
}

void Actor::setPath(std::vector<Vector3>* path)
{
	this->path = path;
}

bool Actor::hasGroup()
{
	return this->isInGroup;
}

void Actor::joinGroup()
{
	this->isInGroup = true;
}

const int& Actor::getHealthRef() const
{
	return this->health;
}

int Actor::getHealth() const
{
	return this->health;
}

int Actor::getMaxHealth() const
{
	return this->updatedStats.maxHealth;
}

void Actor::setHealth(int health)
{
	this->health = std::clamp(health, 0, this->updatedStats.maxHealth);
}

void Actor::setMaxHealth(int maxHealth)
{
	this->updatedStats.maxHealth = max(maxHealth, 1);
}

void Actor::resetHealth()
{
	this->health = this->updatedStats.maxHealth;
}

void Actor::changeHealth(int amount)
{
	this->health = std::clamp(this->health + amount, 0, this->updatedStats.maxHealth);
	Game::getGraphics().addParticle2(this->getPosition(), Vector3(0, 0, 0), 2, 1);
}

bool Actor::isDead() const
{
	return this->health <= 0;
}

void Actor::death()
{
	Game::getGraphics().removeFromDraw(this);
}