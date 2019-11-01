#pragma once
#include "Attacker.h"
#include "Turret.h"
class AIGroup
{
public:
	AIGroup();
	~AIGroup();
	std::vector<Actor*> actors;


	Vector3 getAvaragePos();
	float getGroupRadius();
	void updateAvaragePos();

private:
	float groupRadius = 6.5f * 6.5f; //Compare with distance^2
	Vector3 averagePos;
	std::vector<Vector3> path; 
};

