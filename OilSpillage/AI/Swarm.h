#pragma once
#include "Actor.h"
class Swarm : public Actor
{
public:
	Swarm();
	Swarm(float x, float z, int weaponType);
	~Swarm();
	void update(float dt, Vector3 targetPos);


private:
	void setUpActor();
};

