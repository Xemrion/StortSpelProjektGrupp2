#pragma once
#include "Actor.h"
#include "..//Vehicle.h"
class Spitfire : public Actor
{
private:
	void setUpActor();
	float deltaTime;
	Vehicle* car;
	Vector3 forward;
public:
	Spitfire(float x, float z, AStar* aStar);
	Spitfire();
	~Spitfire();
	void update(float dt, Vector3 targetPos);

	/*if in range lets stop for now*/
	Status inRange();
	Status vehicleUpdate();
	void updateVehicle();
};