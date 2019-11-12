#pragma once
#include "Actor.h"
class DynamicActor : public Actor
{
public:
	DynamicActor();
	DynamicActor(float x,float z);
	virtual ~DynamicActor();
	void setPath(std::vector<Vector3>* path);
	virtual void update(float dt, const Vector3& targetPos);
	void applyForce(const Vector3& force);
	int getBoidOffset()const;
	float getMaxSpeed()const;
	float getMaxForce()const;
	const Vector3& getVelocity()const;

	int duty;
	bool vActive;
	Vector3 groupPos;
protected:	
	int aggroRange;
	int boidOffset;

	Vector3 predictedTargetPos;
	std::vector<Vector3>* path;
	Vector3 destination;
	Vector3 acceleration;
	Vector3 velocity;

	float maxSpeed;
	float maxForce;
	
	enum class State { Idle, Chasing, Circulate };
	State state;

	// one Laws that boids follow
	//Vector3 separation(std::vector<Actor*>& boids, std::vector<Vector3> buildings, Vector3 targetPos = Vector3(0.0f, -100.0f, 0.0f));
	// Other function for moving and interacting
	virtual void move();
	virtual Vector3 seek();
	virtual void moveCirculate(const Vector3& desiredDirection);
	virtual Vector3 seekCirculate(const Vector3& desiredDirection);
	virtual void followPath();
	virtual void circulatePlayer();
	Status setChaseState();
	Status inAggroRange();
	Status setIdleState();
	Status setCirculateState();
};