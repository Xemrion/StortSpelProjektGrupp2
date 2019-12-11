#pragma once
#include"Turret.h"
#include "..//Physics.h"
class TurretHandler
{
private:
	std::vector<Actor*> turrets;
	Physics* physics;
	void destroyTurret(int index);
public:
	TurretHandler();
	TurretHandler(Physics* physics);
	~TurretHandler();
	void update(float dt, const Vector3& targetPos);
	void createTurret(float x, float z,int weaponType,Physics* physics);
	float distanceToPlayer(const Vector3& position);
	void intersectPlayerBullets(Bullet* bulletArray, size_t size, float dt,float soundTimer);


};