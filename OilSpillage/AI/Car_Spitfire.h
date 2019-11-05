#pragma once
#include "Actor.h"
#include "..//Vehicle.h"
class Spitfire : public Actor
{
private:
	void setUpActor();
	float deltaTime;
	Vehicle* car;
	Vector3 direction;
	float gas;
	class Physics* physics;
	Vector3 prevAccelForce;
	Vector3 accelForce;
public:
	Spitfire(float x, float z, AStar* aStar);
	Spitfire();
	~Spitfire();
	void update(float dt, Vector3 targetPos);
	void Init(Physics* physics);
	/*if in range lets stop for now*/
	Status inRange();
	Status vehicleUpdate();
	void updateVehicle();
};