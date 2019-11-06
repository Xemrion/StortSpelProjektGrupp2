#pragma once
#include "Actor.h"
class Attacker : public Actor
{
public:
	Attacker();
	Attacker(float x, float z, int weaponType);
	~Attacker();

	void update(float dt, Vector3 targetPos);
private:
	void setUpActor();
};