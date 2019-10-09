#pragma once
#include "Actor.h"
class Attacker : public Actor
{
public:
	Defender();
	~Defender();

	void update(float dt, Vector3 targetPos);

private:
	void setUpActor();
	Vector3 objectivePos;
	Status inRange();
	//Status attack();
	//Status setChaseState();
	//Status setRoamState();
};
};

