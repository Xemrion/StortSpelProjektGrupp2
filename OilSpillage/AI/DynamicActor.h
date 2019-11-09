#pragma once
#include "Actor.h"
class DynamicActor : public Actor
{
public:
	DynamicActor();
	DynamicActor(float x,float z);
	virtual ~DynamicActor();
	void setPath(std::vector<Vector3>* path);
	virtual void update(float dt,Vector3 targetPos);
protected:	
	int aggroRange;
	int boidOffset;

	std::vector<Vector3>* path;
	Vector3 destination;
	Vector3 acceleration;
	Vector3 velocity;

	float maxSpeed;
	float maxForce;
	
	enum class State { Idle, Chasing };
	State state;

	// one Laws that boids follow
	//Vector3 separation(std::vector<Actor*>& boids, std::vector<Vector3> buildings, Vector3 targetPos = Vector3(0.0f, -100.0f, 0.0f));
	// Other function for moving and interacting
	void applyForce(Vector3 force);
	virtual void move();
	virtual Vector3 seek();
	virtual void followPath();
	Status setChaseState();
	Status enemyNear();
	Status setIdleState();

};