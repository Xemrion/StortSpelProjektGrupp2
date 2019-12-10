#pragma once
#include "Behaviour.h"
#include<vector>
#include "../VehicleStats.h"
#include "../GameObject.h"
#include<SimpleMath.h>
#include<DirectXMath.h>
#include"..//game.h"

using namespace DirectX::SimpleMath;
class Actor : public GameObject
{
public:
	Actor();
	Actor(float x, float z,Physics* physics);
	virtual ~Actor();
	virtual void update(float dt, const Vector3& targetPos);

	float getHealth()const;
	float getMaxHealth()const;
	void setHealth(float health);
	void setMaxHealth(float health);
	void changeHealth(float amount);
	bool isDead() const;
	int getPoints();
	void setPoints(int amount);
	void scaling(float& stat, float ratio);
	void onFire();
	void setFire(float timer);

private:
	float health;
	float particleTimer;
	int points;
	float fireTimer;
protected:
	bool isHit = false;
	float deltaTime;
	Vector3 targetPos;

	Stats stats;

	Vector3 vecForward;
	Selector* root;
	BT bt;
};