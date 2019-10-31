#pragma once
#include "AIGroup.h"
class ActorManager
{
public:
	ActorManager();
	ActorManager(AStar* aStar);
	~ActorManager();
	enum Radius { CLOSE, MIDDLE, OUTER };
	void update(float dt, Vector3 targetPos);
	void createDefender(float x,float z, Vector3 objectivePos = Vector3());
	void createAttacker(float x, float z);
	void createTurret(float x, float z);
	std::vector<AIGroup> groups;
	std::vector<Actor*>* findClosestGroup(Vector3 position);
	float distanceToPlayer(Vector3 position);
	void intersectPlayerBullets(Bullet* bulletArray, size_t size);

	void spawnDefenders(Vector3 objective);
	void spawnAttackers(Vector3 originPos);
	void spawnTurrets(Vector3 position, Radius radius, float angle);
	Vector2& generateRandom(const float& x, const float& z, Radius radius);
	Vector2& generateAroundaPoint(const float& x, const float& z, float angle);
private:
	float soundTimer;
	int frameCount = 0;
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
	std::vector<Vector3> path;
	AStar* aStar;
};