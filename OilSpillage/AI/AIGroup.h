#pragma once
#include "Attacker.h"
#include "Turret.h"
class AIGroup
{
public:
	AIGroup();
	~AIGroup();
	std::vector<Actor*> actors;


	Vector3 getAveragePos();
	float getGroupRadius();
	void updateAveragePos();
	std::vector<Vector3>& getPathRef();
private:
	float groupRadius = 6.5f * 6.5f; //Compare with distance^2
	Vector3 averagePos;
	std::vector<Vector3> path; 
};

