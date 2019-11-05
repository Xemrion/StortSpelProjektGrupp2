#pragma once
#include "Actor.h"
#include "..//Vehicle.h"
class Spitfire : public Actor
{
private:
	bool drive = false;
	void setUpActor();
	Vehicle* car;
public:
	Spitfire(float x, float z, AStar* aStar);
	Spitfire();
	~Spitfire();

	Status idle();
	Status inRange();
	Status rotateTowards();
	Status FollowTarget();



};