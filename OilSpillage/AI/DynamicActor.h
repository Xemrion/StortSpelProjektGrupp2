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
	virtual void knockBack(Vector3 direction, float force);
	void setGroup(void* newGroup);
	void* getGroup()const;
	int duty;
	int pathSize;
	void setStun(float timer);
	void Color();
protected:
	void* curGroup;
	Vector3* path;
	Vector3 destination;
	Vector3 velocity;

	virtual void move();
	virtual Vector3 calculateVelocity();
	virtual void followPath();

};