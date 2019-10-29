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
	this->weapon = WeaponHandler::getWeapon(WeaponType::aiMachineGun);
}

Actor::Actor(float x, float z, AStar* aStar = nullptr)
{
	this->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(this);

	this->aStar = aStar;
	this->setUpActor();
	this->timeSinceLastShot = 0;
	this->acceleration = Vector3(0.0f);
	this->velocity = Vector3(10.0f, 0.0f, 10.0f);
	this->position = Vector3(x, -1.0f, z);
	this->maxSpeed = 3.5f;
	this->maxForce = 0.5f;
	this->weapon = WeaponHandler::getWeapon(WeaponType::aiMachineGun);

	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
}

Actor::~Actor()
{

}

void Actor::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;


	if (nrOfFrames % 100 == 0)
	{

		//findPath();
		nrOfFrames = 1;
	}

	this->root->func();
	followPath();

	nrOfFrames++;
}

void Actor::updateWeapon(float deltaTime)
{
	this->timeSinceLastShot += deltaTime;

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
				dir = (targetPos - bulletOrigin);

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

	for (int i = 0; i < Actor::bulletCount; i++)
	{
		bullets[i].update(deltaTime);
	}
}

Status Actor::shoot()
{
	
	return Status::SUCCESS;
}

void Actor::chase()
{
	//findPath();
	//Vector3 dir = targetPos - this->getPosition();
	//dir.Normalize();
	//Vector3 newPosition = this->getPosition() + dir * deltaTime;
	//this->setPosition(newPosition);

	//for (int i = 0; i < boids.size(); i++) //Updating Boids
	//{
	//	boids.at(i)->setDestination(targetPos);
	//}
}

void Actor::roam()
{
	//Hitta random position och hitta väg till den.

	//findPath();
}

Status Actor::inRange()
{
	Status status;

	if ((getPosition() - targetPos).Length() > 7)
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
	if (path.size() > 0)
	{
		destination = path.at(path.size() - 1);
		if (position.Distance(path.at(path.size() - 1), position) < 2)
		{
			path.pop_back();
		}
	}
	else
	{
		if (this->state == State::Chasing)
		{
			chase();
		}
		else if (this->state == State::Roaming)
		{
			chase();
			//roam();
		}
	}
}

void Actor::findPath()
{
	aStar->algorithm(this->getPosition(), destination, path);
}

void Actor::applyForce(Vector3 force)
{
	acceleration += force;
}

Vector3 Actor::separation(vector<Actor*> boids, Vector3 targetPos)
{
	// Distance of field of vision for separation between boids
	float desiredSeparationDistance = 3.0f * 3.0f;
	Vector3 direction(0.0f);
	float nrInProximity = 0.0f;
	// For every boid in the system, check if it's too close
	for (int i = 0; i < boids.size(); i++)
	{
		// Calculate distance from current boid to boid we're looking at
		Vector3 curBoidPos = boids.at(i)->position;
		float deltaX = position.x - curBoidPos.x;
		float deltaZ = position.z - curBoidPos.z;
		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		// If this is a fellow boid and it's too close, move away from it
		if (this != boids.at(i) && (distance < desiredSeparationDistance))
		{
			Vector3 difference(0.0f);
			difference = position - boids.at(i)->position;
			difference.Normalize();
			if (distance != 0)
			{
				difference /= distance;      // Weight by distance
			}
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

Vector3 Actor::alignment(vector<Actor*> boids)
{
	float neighborDistance = 20.0f * 20.0f; // Field of vision
	Vector3 sum(0.0f);
	float nrInProximity = 0.0f;
	for (int i = 0; i < boids.size(); i++)
	{

		Vector3 curBoidPos = boids.at(i)->position;
		float deltaX = position.x - curBoidPos.x;
		float deltaZ = position.z - curBoidPos.z;
		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		if ((distance > 0.0f) && (distance < neighborDistance))
		{
			sum += boids.at(i)->velocity;
			nrInProximity++;
		}
	}
	// If there are boids close enough for alignment...
	if (nrInProximity > 0.0f)
	{
		sum /= nrInProximity;// Divide sum by the number of close boids (average of velocity)
		sum.Normalize();            // Turn sum into a unit vector, and
		sum *= maxSpeed;    // Multiply by maxSpeed
		// Steer = Desired - Velocity
		Vector3 direction;
		direction = (sum - velocity); //sum = desired(average)
		if (direction.Length() > maxForce)
		{
			direction /= direction.Length();
		}
		return direction;
	}
	else
	{
		Vector3 temp(0.0f);
		return temp;
	}
}

Vector3 Actor::cohesion(vector<Actor*> boids)
{
	float neighborDistance = 2.0f * 2.0f;
	Vector3 sum(0.0f);
	float nrInProximity = 0.0f;
	for (int i = 0; i < boids.size(); i++)
	{
		Vector3 curBoidPos = boids.at(i)->position;
		float deltaX = position.x - curBoidPos.x;
		float deltaZ = position.z - curBoidPos.z;
		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		if ((distance > 0.0f) && (distance < neighborDistance))
		{
			sum += boids.at(i)->position;
			nrInProximity++;
		}
	}
	if (nrInProximity > 0.0f)
	{
		sum /= nrInProximity;
		return seek(sum);
	}
	else
	{
		Vector3 temp(0.0f);
		return temp;
	}
}

Vector3 Actor::seek(Vector3 target)
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

void Actor::run(vector<Actor*> boids, float deltaTime, Vector3 targetPos)
{
	seek(targetPos);
	applyForce(separation(boids, targetPos)*4);
	update(deltaTime, targetPos);
	updateBoid(deltaTime);
}

void Actor::updateBoid(float deltaTime)
{
	//To make the slow down not as abrupt
	acceleration *= 0.4f;
	// Update velocity
	velocity += acceleration;
	velocity += seek(Vector3(0.0f));
	// Limit speed
	if (velocity.Length() > maxForce)
	{
		velocity /= velocity.Length();
	}
	Vector3 temp = position + Vector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime) * 3;
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

void Actor::flock(vector<Actor*> boids, Vector3 targetPos)
{
	Vector3 seperationForce = separation(boids, targetPos);
	Vector3 alignmentForce = alignment(boids);
	Vector3 cohesionForce = cohesion(boids);
	// Arbitrarily weight these forces
	seperationForce *= 3.0f;
	alignmentForce *= 0.0f; // Might need to alter weights for different characteristics
	cohesionForce *= 0.0f;

	// Add the force vectors to acceleration
	applyForce(seperationForce);
	applyForce(alignmentForce);
	applyForce(cohesionForce);
}

float Actor::angle(Vector3 target)
{
	// From the definition of the dot product
	float angle = (float)(atan2(target.x, -target.z) * 180 / 3.14);
	return angle;
}

void Actor::setPath(std::vector<Vector3> path)
{
	this->path = path;
}

Vector3 Actor::getDestination()
{
	return destination;
}

bool Actor::hasGroup()
{
	return this->isInGroup;
}

void Actor::setDestination(Vector3 destination)
{
	this->destination = destination;
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
	Game::getGraphics().addParticle2(this->getPosition() + Vector3(0, 3, 0), Vector3(0, 0, 0), 2, 1);
}

bool Actor::isDead() const
{
	return this->health <= 0;
}

void Actor::death()
{
	Game::getGraphics().removeFromDraw(this);
}