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
	std::vector<std::vector<Actor*>> groups;
private:
	void updateAveragePos();
	//Returns index for the group within the radius with the most members
	int groupInRange(Vector3 actorPos,int currentGroupSize);
	void joinGroup(Actor* actor,int groupIndex);
	void leaveGroup(int groupIndex, int where);
	float groupRadius = 5.5f;
	void updateGroups();
	void createGroup(Actor* actor);
	std::vector<Actor*> actors;
	std::vector<Vector3> averagePos;
	AStar* aStar;
};