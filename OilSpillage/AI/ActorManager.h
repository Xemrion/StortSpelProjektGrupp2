#pragma once
#include "AIGroup.h"
class Physics;
#include "TurretHandler.h"
#include "AStar.h"
#include "..//PG/Map.hpp"
#include "Boss.h"

class ActorManager
{
public:
	ActorManager();
	ActorManager(AStar* aStar,Physics* physics, Map* map, std::mt19937* RNG);
	~ActorManager();
	enum Radius { CLOSE, MIDDLE, OUTER };
	void update(float dt, const Vector3& targetPos);
	void createAttacker(float x, float z, int weaponType = 0);
	void createSniper(float x, float z, int weaponType = 0);
	void createTurret(float x, float z, int weaponType = 0);
	void createSwarm(float x, float z);
	Boss* createBoss(float x, float z, int weaponType);

	void createSpitFire(float x, float z);
	void createChaseCar(float x, float z);
	void createShootCar(float x, float z, int weaponType = 0);

	float distanceToPlayer(const Vector3& position);
	const std::vector<AIGroup>& getGroups() const;
	void spawnAttackers(const Vector3& originPos);
	void spawnSnipers(const Vector3& originPos);
	void spawnChaseCars(const Vector3& originPos);
	void spawnShootCars(const Vector3& originPos);
	void spawnSwarm(const Vector3& originPos);
	void spawnTurrets(const Vector3& position, Radius radius, float angle);

	void seperation(const Vector3& targetPos);
	void intersectPlayerBullets(Bullet* bulletArray, size_t size);
private:
	const int maxNrOfEnemies = 25;
	const int spawnCooldown = 2;
	float spawnTimer = 0;
	float soundTimer = 0;
	int frameCount = 0;
	Physics* physics;
	Map* map;
	std::mt19937* rng;
	void updateAveragePos();
	void updateActors(float dt, Vector3 targetPos);
	//Returns index for the group within the radius with the most members
	int groupInRange(const Vector3& actorPos, int currentGroupSize);
	void joinGroup(DynamicActor* actor, int groupIndex);
	void spawnEnemies(const Vector3& targetPos);
	void leaveGroup(int groupIndex, int where);
	void assignPathsToGroups(const Vector3& targetPos);
	void updateGroups();
	void destroyActor(int index);
	void initGroupForActor(DynamicActor* actor);
	void createGroup(DynamicActor* actor);
	Vector3 predictPlayerPos(const Vector3& targetPos);
	Vector3 generateObjectivePos(const Vector3& targetPos, float minDistance, float maxDistance) noexcept;
	std::vector<AIGroup> groups;
	Vector2& generateRandom(const float& x, const float& z, Radius radius);
	Vector2& generateAroundaPoint(const float& x, const float& z, float angle);
	std::vector<DynamicActor*> actors;
	TurretHandler turretHandler;
	std::vector<DynamicActor*> sendToPlayer;
	AStar* aStar;
};