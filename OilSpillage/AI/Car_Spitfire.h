#pragma once
#include "DynamicActor.h"
#include "AIVehicle.h"
class Spitfire : public DynamicActor
{
private:
	void setUpActor();

	AIVehicle* car;
	Vector3 direction;
	float throttleInputStrength;
	Vector3 prevAccelForce;
	Vector3 accelForce;
	void updateVehicle(); 
public:
	Spitfire(float x, float z,Physics* physics);
	Spitfire();
	virtual ~Spitfire();
	void move();
	void update(float dt, Vector3 targetPos);
	void followPath();
};