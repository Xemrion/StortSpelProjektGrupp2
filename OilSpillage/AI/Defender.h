#pragma once
#include "Actor.h"
class Defender :
	public Actor
{

public:
	Defender();
	~Defender();

	void update(float dt, Vector3 targetPos);

private:
	void setUpActor();
	Vector3 objectivePos;

	//Status attack();
	Status inRange();
	//Status setChaseState();
	//Status setRoamState();
};

