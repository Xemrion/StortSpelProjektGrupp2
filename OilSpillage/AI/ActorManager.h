#pragma once
#include "AIGroup.h"
#include "TurretHandler.h"
#include "Swarm.h"
class ActorManager
{
public:
	ActorManager();
	ActorManager(AStar* aStar);
	~ActorManager();
	enum Radius { CLOSE, MIDDLE, OUTER };
	void update(float dt, Vector3 targetPos);
	void createAttacker(float x, float z, int weaponType = 1);
	void createTurret(float x, float z, int weaponType = 1);
	void createSwarm(float x, float z, int weaponType = 1);
	std::vector<AIGroup> groups;
	float distanceToPlayer(Vector3 position);
	void intersectPlayerBullets(Bullet* bulletArray, size_t size);

	void spawnAttackers(Vector3 originPos, int weaponType);
	void spawnTurrets(Vector3 position, Radius radius, float angle, int weaponType);
	Vector2& generateRandom(const float& x, const float& z, Radius radius);
	Vector2& generateAroundaPoint(const float& x, const float& z, float angle);
private:
	float soundTimer = 0;
	int frameCount = 0;
	void updateAveragePos();
	//Returns index for the group within the radius with the most members
	int groupInRange(Vector3 actorPos, int currentGroupSize);
	void joinGroup(Actor* actor, int groupIndex);
	void leaveGroup(int groupIndex, int where);
	void assignPathsToGroups(Vector3 targetPos);
	void updateGroups();
	void initGroupForActor(Actor* actor);
	void createGroup(Actor* actor);
	std::vector<Actor*> actors;
	TurretHandler turretHandler;
	std::vector<Actor*> sendToPlayer;
	AStar* aStar;
};