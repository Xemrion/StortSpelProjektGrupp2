#include "Boid.h"

Boid::Boid(float x, float z)
{
	acceleration = Vector3(0.0f);
	velocity = Vector3(10.0f, 0.0f, 10.0f);
	location = Vector3(x, 0, z);
	maxSpeed = 3.5;
	maxForce = 0.5;

	desiredPosition = Vector3(100.0f, 0.0f, 100.0f);
}

void Boid::applyForce(Vector3 force)
{
	acceleration += force;
}

Vector3 Boid::Separation(vector<Boid*> Boids)
{
	// Distance of field of vision for separation between boids
	float desiredseparationDistance = 3;
	Vector3 direction(0.0f);
	int count = 0;
	// For every boid in the system, check if it's too close
	for (int i = 0; i < Boids.size(); i++) {
		// Calculate distance from current boid to boid we're looking at
		float distance = (location - Boids.at(i)->location).Length();
		// If this is a fellow boid and it's too close, move away from it
		if ((distance > 0) && (distance < desiredseparationDistance)) {
			Vector3 difference(0.0f);
			difference = location - Boids.at(i)->location;
			difference.Normalize();
			difference /= distance;      // Weight by distance
			direction += difference;
			count++;
		}
	}
	// Adds average difference of location to acceleration
	if (count > 0)
		direction /= (float)count;
	if (direction.Length() > 0) {
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

Vector3 Boid::Alignment(vector<Boid*> Boids)
{
	float neighbordistance = 10; // Field of vision
	Vector3 desiredposition = Vector3(10.0f, 0.0f,0.0f);
	Vector3 sum(0.0f);
	int count = 0;
	for (int i = 0; i < Boids.size(); i++) {
		float distance = (location - Boids.at(i)->location).Length();
		if ((distance > 0) && (distance < neighbordistance)) { // 0 < d < 50
			sum += Boids.at(i)->velocity;
			count++;
		}
	}
	// If there are boids close enough for alignment...
	if (count > 0) {
		sum /= (float)count;// Divide sum by the number of close boids (average of velocity)
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
	else {
		Vector3 temp(0.0f);
		return temp;
	}
}

Vector3 Boid::Cohesion(vector<Boid*> Boids)
{
	float neighbordistance = 3;
	Vector3 sum(0.0f);
	int count = 0;
	for (int i = 0; i < Boids.size(); i++) {
		float distance = (location - Boids.at(i)->location).Length();
		if ((distance > 0) && (distance < neighbordistance)) {
			sum += Boids.at(i)->location;
			count++;
		}
	}
	if (count > 0) {
		sum /= count;
		return seek(sum);
	}
	else {
		Vector3 temp(0.0f);
		return temp;
	}
}

Vector3 Boid::seek(Vector3 target)
{
	Vector3 desired;
	desired -= location - desiredPosition;
	desired -= target;
	desired *= maxSpeed;

	acceleration = desired - velocity;
	if (acceleration.Length() > maxForce)
	{
		acceleration /= acceleration.Length();
	}
	return acceleration;
}

void Boid::run(vector<Boid*> Boids, float deltaTime)
{
	flock(Boids);
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
	location += Vector3(velocity.x * deltaTime, 0.0f, velocity.z* deltaTime);
	this->move(Vector3(velocity.x * deltaTime, 0.0f, velocity.z*deltaTime));
	// Reset accelertion to 0 each cycle
	acceleration *= 0;
}

void Boid::flock(vector<Boid*> Boids)
{
	Vector3 seperationForce = Separation(Boids);
	Vector3 alignmentForce = Alignment(Boids);
	Vector3 cohesionForce = Cohesion(Boids);
	// Arbitrarily weight these forces
	seperationForce *= 1.2;
	alignmentForce *= 2.0; // Might need to alter weights for different characteristics
	cohesionForce *= 2.0;

	// Add the force vectors to acceleration
	applyForce(seperationForce);
	applyForce(alignmentForce);
	applyForce(cohesionForce);
}

float Boid::angle(Vector3 target)
{
	// From the definition of the dot product
	float angle = 0;//(float)(atan2(v.x, -v.y) * 180 / PI);
	return angle;
}

Vector3 Boid::getLocation()
{
	return this->location;
}
