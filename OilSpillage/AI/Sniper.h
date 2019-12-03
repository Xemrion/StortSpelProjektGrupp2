#pragma once
#include "DynamicActor.h"
#include "Ranged.h"
class Sniper : public DynamicActor, public Ranged
{
public:
	Sniper();
	Sniper(float x, float z, int weaponType, Physics* physics);
	virtual ~Sniper();
	void update(float dt, const Vector3& targetPos);

private:
	void setUpActor();
	Vector3 calculateVelocity();
};