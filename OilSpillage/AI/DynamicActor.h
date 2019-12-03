#pragma once
#include "Actor.h"
class DynamicActor : public Actor
{
public:
	DynamicActor();
	DynamicActor(float x,float z, Physics* physics);
	virtual ~DynamicActor();
	void setPath(Vector3* path);
	virtual void update(float dt, const Vector3& targetPos);

	int duty; 
	bool vActive;
	Vector3* path;
	int pathSize;
protected:	
	int aggroRange;
	Vector3 destination;
	Vector3 velocity;

	
	enum class State { Idle, Chasing };
	//State state;

	virtual void move();
	virtual Vector3 calculateVelocity();
	virtual void followPath();
	Status setChaseState();
	Status inAggroRange();
	Status setIdleState();
};