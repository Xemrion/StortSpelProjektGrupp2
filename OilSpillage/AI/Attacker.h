#pragma once
#include"Ranged.h"
#include"DynamicActor.h"
class Attacker : public DynamicActor, public Ranged
{
public:
	Attacker();
	Attacker(float x, float z, int weaponType);
	virtual ~Attacker();
	void update(float dt, Vector3 targetPos);
private:
	void setUpActor();
	Vector3 seek();
};