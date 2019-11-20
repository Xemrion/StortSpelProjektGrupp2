#pragma once
#include "DynamicActor.h"
#include"Melee.h"
class Swarm : public DynamicActor,public Melee
{
public:
	Swarm();
	Swarm(float x, float z, Physics* physics);
	~Swarm();
	void update(float dt, const Vector3& targetPos);

private:
	void setUpActor();
	Vector3 seek();
};

