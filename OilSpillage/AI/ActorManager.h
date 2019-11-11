#pragma once
#include "AIGroup.h"
class Physics;
#include "TurretHandler.h"
#include "AStar.h"
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
	void createBoss(float x, float z, int weaponType = 1);

	void createSpitFire(float x, float z, Physics* physics);
	std::vector<AIGroup> groups;
	float distanceToPlayer(Vector3 position);
	void intersectPlayerBullets(Bullet* bulletArray, size_t size);

	void spawnAttackers(Vector3 originPos, int weaponType);
	void spawnTurrets(Vector3 position, Radius radius, float angle, int weaponType);
	void spawnBoss(Vector3 originPos, int weaponType);
	Vector2& generateRandom(const float& x, const float& z, Radius radius);
	Vector2& generateAroundaPoint(const float& x, const float& z, float angle);
	void seperation(Vector3 targetPos);
private:
	float soundTimer = 0;
	int frameCount = 0;
	void updateAveragePos();
	//Returns index for the group within the radius with the most members
	int groupInRange(Vector3 actorPos, int currentGroupSize);
	void joinGroup(DynamicActor* actor, int groupIndex);
	void leaveGroup(int groupIndex, int where);
	void assignPathsToGroups(Vector3 targetPos);
	void updateGroups();
	void initGroupForActor(DynamicActor* actor);
	void createGroup(DynamicActor* actor);
	std::vector<DynamicActor*> actors;
	TurretHandler turretHandler;
	std::vector<DynamicActor*> sendToPlayer;
	AStar* aStar;
};