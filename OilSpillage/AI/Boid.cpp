#include "Boid.h"

Boid::Boid()
{
	this->acceleration = Vector3(0.0f);
	this->velocity = Vector3(10.0f, 0.0f, 10.0f);
	this->position = Vector3(0, 0, 0);
	this->maxSpeed = 3.5;
	this->maxForce = 0.5;

	this->destination = Vector3(0.0f, 0.0f, 0.0f);
}

Boid::Boid(float x, float z)
{
	this->acceleration = Vector3(0.0f);
	this->velocity = Vector3(10.0f, 0.0f, 10.0f);
	this->position = Vector3(x, 0, z);
	this->maxSpeed = 3.5;
	this->maxForce = 0.5;

	this->destination = Vector3(20.0f, 0.0f, 20.0f);
}

void Boid::applyForce(Vector3 force)
{
	acceleration += force;
}

Vector3 Boid::separation(vector<Boid*> boids)
{
	// Distance of field of vision for separation between boids
	float desiredSeparationDistance = 1.5;
	Vector3 direction(0.0f);
	int nrInProximity = 0;
	// For every boid in the system, check if it's too close
	for (int i = 0; i < boids.size(); i++) 
	{
		// Calculate distance from current boid to boid we're looking at
		float distance = (position - boids.at(i)->position).Length();
		// If this is a fellow boid and it's too close, move away from it
		if ((distance > 0) && (distance < desiredSeparationDistance)) 
		{
			Vector3 difference(0.0f);
			difference = position - boids.at(i)->position;
			difference.Normalize();
			difference /= distance;      // Weight by distance
			direction += difference;
			nrInProximity++;
		}
	}
	// Adds average difference of location to acceleration
	if (nrInProximity > 0)
	{
		direction /= (float)nrInProximity;
	}
	if (direction.Length() > 0) 
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

Vector3 Boid::alignment(vector<Boid*> boids)
{
	float neighborDistance = 20; // Field of vision
	Vector3 sum(0.0f);
	int nrInProximity = 0;
	for (int i = 0; i < boids.size(); i++) 
	{
		float distance = (position - boids.at(i)->position).Length();
		if ((distance > 0) && (distance < neighborDistance)) 
		{
			sum += boids.at(i)->velocity;
			nrInProximity++;
		}
	}
	// If there are boids close enough for alignment...
	if (nrInProximity > 0) 
	{
		sum /= (float)nrInProximity;// Divide sum by the number of close boids (average of velocity)
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

Vector3 Boid::cohesion(vector<Boid*> boids)
{
	float neighborDistance = 2;
	Vector3 sum(0.0f);
	int nrInProximity = 0;
	for (int i = 0; i < boids.size(); i++) 
	{
		float distance = (position - boids.at(i)->position).Length();
		if ((distance > 0) && (distance < neighborDistance)) 
		{
			sum += boids.at(i)->position;
			nrInProximity++;
		}
	}
	if (nrInProximity > 0) 
	{
		sum /= float(nrInProximity);
		return seek(sum);
	}
	else 
	{
		Vector3 temp(0.0f);
		return temp;
	}
}

Vector3 Boid::seek(Vector3 target)
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

void Boid::run(vector<Boid*> boids, float deltaTime)
{
	flock(boids);
	update(deltaTime);
}

void Boid::update(float deltaTime)
{
	//To make the slow down not as abrupt
	acceleration *= 0.4f;
	// Update velocity
	velocity += acceleration;
	// Limit speed
	if (velocity.Length() > maxForce)
	{
		velocity /= velocity.Length();
	}
	position += Vector3(velocity.x * deltaTime, 0.0f, velocity.z* deltaTime);
	this->move(Vector3(velocity.x * deltaTime, 0.0f, velocity.z*deltaTime));
	// Reset accelertion to 0 each cycle
	acceleration *= 0;
}

void Boid::flock(vector<Boid*> boids)
{
	Vector3 seperationForce = separation(boids);
	Vector3 alignmentForce = alignment(boids);
	Vector3 cohesionForce = cohesion(boids);
	// Arbitrarily weight these forces
	seperationForce *= 3.0f;
	alignmentForce *= 1.4f; // Might need to alter weights for different characteristics
	cohesionForce *= 1.2f;

	// Add the force vectors to acceleration
	applyForce(seperationForce);
	applyForce(alignmentForce);
	applyForce(cohesionForce);
}

float Boid::angle(Vector3 target)
{
	// From the definition of the dot product
	float angle = (float)(atan2(target.x, -target.z) * 180 / 3.14);
	return angle;
}

Vector3 Boid::getDestination()
{
	return destination;
}

void Boid::setDestination(Vector3 destination)
{
	this->destination = destination;
}
