
#pragma once
#include "Actor.h"

class Turret : public Actor
{
public:
	Turret();
	Turret(float x, float z);
	void update(float dt, Vector3 targetPos);
private:
	void setForwardVector(Vector3 forward);
	void setSightRange(int range);
	void setUpActor();
	void followPath();
	void updateBoid(float deltatime);
	Status rotateTowards();
	Status inRange();
	Status idle();

	Vector3 vecForward;
	int sightRange;
};
