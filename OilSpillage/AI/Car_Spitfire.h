#pragma once
#include "Actor.h"
#include "..//Vehicle.h"
class Spitfire : public Actor
{
private:
	void setUpActor();
	float deltaTime;

	//car functions
	Vehicle* car;
	Vector3 direction;
	float throttleInputStrength;
	class Physics* physics;
	Vector3 prevAccelForce;
	Vector3 accelForce;
	void updateVehicle(); 
public:
	Spitfire(float x, float z, AStar* aStar);
	Spitfire();
	~Spitfire();
	void update(float dt, Vector3 targetPos);
	void Init(Physics* physics);
	/*if in range lets stop for now*/
	Status inRange();
	Status vehicleUpdate();
};