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
	void initGroups();
private:
	void updateGroups();
	std::vector<Actor*> actors;
	std::vector<std::vector<Actor*>> groups;
	std::vector<Vector3> averagePos;
	AStar* aStar;
};