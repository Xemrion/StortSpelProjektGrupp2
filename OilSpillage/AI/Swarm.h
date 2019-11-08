#pragma once
#include "DynamicActor.h"
#include"Melee.h"
class Swarm : public DynamicActor,public Melee
{
public:
	Swarm();
	Swarm(float x, float z, int weaponType);
	~Swarm();

private:
	void setUpActor();
};

