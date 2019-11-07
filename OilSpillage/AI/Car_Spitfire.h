#pragma once
#include "Actor.h"
#include "AIVehicle.h"
class Spitfire : public Actor
{
private:
	void setUpActor();

	//car functions
	AIVehicle* car;
	Vector3 direction;
	float throttleInputStrength;
	Vector3 prevAccelForce;
	Vector3 accelForce;
	void updateVehicle(); 
	int chaseRange = 500;
public:
	Spitfire(float x, float z,Physics* physics);
	Spitfire();
	~Spitfire();
	void move();
	void update(float dt, Vector3 targetPos);
	void followPath();

	/*if in range lets stop for now*/
	Status inRange();
	Status vehicleUpdate();
};