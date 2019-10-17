#pragma once
#include "Actor.h"
class Defender : public Actor
{

public:
	Defender();
	Defender(float x, float z, AStar* aStar,Vector3 objectivePos = Vector3());
	~Defender();

	void update(float dt, Vector3 targetPos);

private:
	void setUpActor();
	void followPath();
	void returning();
	Vector3 objectivePos;

	//Status attack();
	Status inRange();
	Status inObjectiveRange();
	Status idle();
	Status setReturnState();
	Status enemyWhithinObjective();
	//Status setChaseState();
	//Status setRoamState();
};

