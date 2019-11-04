
#pragma once
#include "Actor.h"

class Turret : public Actor
{
public:
	Turret();
	Turret(float x, float z);
	~Turret();
	void update(float dt, Vector3 targetPos);
private:
	int deltaTime;
	void setForwardVector(Vector3 forward);
	void setSightRange(int range);
	void setUpActor();
	Status rotateTowards();
	Status inRange();
	Status idle();
	Status lineOfSight();
	GameObject body;
	int sightRange;

	Vector3 idleTarget;
	float turretAngle;

	void calculateTarget(float& angle);
};
