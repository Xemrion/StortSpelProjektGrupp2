#pragma once
#include "DynamicActor.h"
#include"Melee.h"
class Swarm : public DynamicActor,public Melee
{
public:
	Swarm();
	Swarm(float x, float z, Physics* physics);
	virtual ~Swarm();
	void update(float dt, const Vector3& targetPos);
protected:
	void createRigidbody(Physics* physics);
private:
	void setUpActor();
	Vector3 calculateVelocity();
};

