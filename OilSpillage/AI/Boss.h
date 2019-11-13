#pragma once
#include "Ranged.h"
#include "DynamicActor.h"
class Boss : public DynamicActor, public Ranged
{
public:
	Boss();
	Boss(float x, float z, int weaponType);
	virtual ~Boss();
	void update(float dt, const Vector3& targetPos);
private:
	void setUpActor();
	int testNr;
};