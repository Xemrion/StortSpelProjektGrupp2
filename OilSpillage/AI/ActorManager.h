#pragma once
#include "Defender.h"
#include "Attacker.h"
class ActorManager
{
public:
	ActorManager();
	~ActorManager();
	void update(float dt, Vector3 targetPos);
	void createDefender(float x,float z);
	void createAttacker(float x, float z);
	void setAStar(AStar* aStar);
private:
	std::vector<Actor*> actors;
	std::vector<Actor*> group1;
	AStar* aStar;
};