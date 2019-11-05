#pragma once
#include"Turret.h"
class TurretHandler
{
private:
	std::vector<Actor*> turrets;
public:
	TurretHandler();
	~TurretHandler();
	void update(float dt, DirectX::SimpleMath::Vector3 targetPos);
	void createTurret(float x, float z,int weaponType);
	float distanceToPlayer(DirectX::SimpleMath::Vector3 position);
	void intersectPlayerBullets(Bullet* bulletArray, size_t size,float soundTimer);


};