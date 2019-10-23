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
}

Actor::~Actor()
{

}

void Actor::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;

	if(nrOfFrames % 100 == 0)
	{
		findPath();
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
				this->bullets[i].shoot(bulletOrigin,
					dir,
					this->velocity);
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
	findPath();
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

	findPath();
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
		targetNode = DirectX::SimpleMath::Vector3(float(path.at(path.size() - 1)->getXPos()),
			.0f,
			float(path.at(path.size() - 1)->getYPos()));
		Vector3 dir = targetNode - this->getPosition();
		dir.Normalize();

		destination = targetNode;


	if (position.Distance(targetNode, position) < 1)
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
			roam();
		}
	}
}

void Actor::findPath()
{
	aStar->algorithm(this->getPosition(), targetPos, path);
}

void Actor::applyForce(Vector3 force)
{
	acceleration += force;
}

Vector3 Actor::separation(vector<Actor*> boids, Vector3 targetPos)
{
	// Distance of field of vision for separation between boids
	float desiredSeparationDistance = 3.0f;
	Vector3 direction(0.0f);
	float nrInProximity = 0.0f;
	// For every boid in the system, check if it's too close
	for (int i = 0; i < boids.size(); i++)
	{
		// Calculate distance from current boid to boid we're looking at
		float distance = (position - boids.at(i)->position).Length();
		// If this is a fellow boid and it's too close, move away from it
		if (this != boids.at(i) && (distance < desiredSeparationDistance))
		{
			Vector3 difference(0.0f);
			difference = position - boids.at(i)->position;
			difference.Normalize();
			difference /= distance;      // Weight by distance
			direction += difference;
			nrInProximity++;
		}
	}
	// Calculate distance from current boid to player
	float distance = (position - targetPos).Length();
	// If this is a fellow boid and it's too close, move away from it
	if (distance < desiredSeparationDistance)
	{
		Vector3 difference(0.0f);
		difference = position - targetPos;
		difference.Normalize();
		difference /= distance;      // Weight by distance
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
	float neighborDistance = 20.0f; // Field of vision
	Vector3 sum(0.0f);
	float nrInProximity = 0.0f;
	for (int i = 0; i < boids.size(); i++)
	{
		float distance = (position - boids.at(i)->position).Length();
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
	float neighborDistance = 2.0f;
	Vector3 sum(0.0f);
	float nrInProximity = 0.0f;
	for (int i = 0; i < boids.size(); i++)
	{
		float distance = (position - boids.at(i)->position).Length();
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
	flock(boids, targetPos);
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
	position += Vector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime);
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

void Actor::setPath(std::vector<Node*> path)
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
