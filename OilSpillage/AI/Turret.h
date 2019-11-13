#pragma once
#include "Ranged.h"
#include "Actor.h"
class Turret : public Actor,public Ranged
{
public:
	Turret();
	Turret(float x, float z, int weaponType);
	virtual ~Turret();
	void update(float dt, const Vector3& targetPos);
private:
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

	void calculateTarget(float angle);
};
