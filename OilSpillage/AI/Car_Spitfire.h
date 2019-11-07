#pragma once
#include "Actor.h"
#include "..//Vehicle.h"
class Spitfire : public Actor
{
private:
	void setUpActor();

	//car functions
	Vehicle* car;
	Vector3 direction;
	float throttleInputStrength;
	Vector3 prevAccelForce;
	Vector3 accelForce;
	void updateVehicle(); 
	int chaseRange = 1000;
public:
	Spitfire(float x, float z,Physics* physics);
	Spitfire();
	~Spitfire();
	void move();
	void followPath();

	/*if in range lets stop for now*/
	Status inRange();
	Status vehicleUpdate();
};