#pragma once
#include "Dynamic.h"
#include"Melee.h"
class Swarm : public Dynamic,public Melee
{
public:
	Swarm();
	Swarm(float x, float z, int weaponType);
	~Swarm();

private:
	void setUpActor();
};

