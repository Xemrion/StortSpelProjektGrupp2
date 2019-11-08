#pragma once
#include "Actor.h"
class Dynamic : public Actor
{
public:
	Dynamic();
	Dynamic(float x,float z);
	virtual ~Dynamic();
	void setPath(std::vector<Vector3>* path);
	virtual void update(float dt,Vector3 targetPos);
protected:	
	int aggroRange = 20;
	virtual void move();
	std::vector<Vector3>* path;
	Vector3 destination;
	Vector3 acceleration;
	float maxSpeed;
	float maxForce;
	Vector3 velocity;
	enum State { Idle, Chasing };
	State state;

	void applyForce(Vector3 force);
	// one Laws that boids follow
	//Vector3 separation(std::vector<Actor*>& boids, std::vector<Vector3> buildings, Vector3 targetPos = Vector3(0.0f, -100.0f, 0.0f));
	// Other function for moving and interacting
	virtual Vector3 seek();
	virtual void  followPath();
	virtual Status setChaseState();
	virtual Status enemyNear();
	virtual Status setIdleState();


	int boidOffset;

};