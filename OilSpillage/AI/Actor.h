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
	virtual void setUpActor() = 0 {};

	void setHealth(float health);
	void changeHealth(float amount);
	bool isDead() const;
	int getPoints();
	void setPoints(int amount);

private:
	float health;
	int points;
protected:
	bool isHit = false;
	float deltaTime;
	Vector3 targetPos;

	Stats stats;

	Vector3 vecForward;
	Selector* root;
	BT bt;
};