
#pragma once
#ifndef  AITURRENT_H
#define  AITURRENT_H
#include "Actor.h"

class Turrent :public Actor
{
public:
	Turrent();
	Turrent(float x, float z, AStar* aStar);
	void update(float dt, Vector3 targetPos);
private:
	void setForwardVector(Vector3 forward);
	void setSightRange(int range);
	void setUpActor();
	void followPath();

	Status rotateToWards();
	Status inRange();
	Status idle();

	Vector3 vecForward;
	int sightRange;
};
#endif // ! AITURRENT_H