#pragma once
#include "Defender.h"
#include "Attacker.h"
#include "Turret.h"
class ActorManager
{
public:
	ActorManager();
	ActorManager(AStar* aStar);
	~ActorManager();
	void update(float dt, Vector3 targetPos);
	void createDefender(float x,float z, Vector3 objectivePos = Vector3());
	void createAttacker(float x, float z);
	void createTurret(float x, float z);
	std::vector<std::vector<Actor*>> groups;
	std::vector<Actor*>* findClosestGroup(Vector3 position);

	void spawnDefenders(std::vector<Vector3> objectives);
	void spawnAttackers(Vector3 playerPos);
private:
	int frameCount = 0;
	float groupRadius = 6.5f * 6.5f; //Compare with distance^2
	void updateAveragePos();
	//Returns index for the group within the radius with the most members
	int groupInRange(Vector3 actorPos,int currentGroupSize);
	void joinGroup(Actor* actor,int groupIndex);
	void leaveGroup(int groupIndex, int where);
	void assignPathsToGroups(Vector3 targetPos);
	void updateGroups();
	void initGroupForActor(Actor* actor);
	void createGroup(Actor* actor);
	std::vector<Actor*> actors;
	std::vector<Actor*> sendToPlayer;
	std::vector<Vector3> averagePos;
	AStar* aStar;
};