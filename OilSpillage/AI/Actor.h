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
	Actor(float x, float z);
	virtual ~Actor();
	virtual void update(float dt, Vector3 targetPos);
	virtual void setUpActor() = 0 {};

	void setHealth(int health);
	void changeHealth(int amount);
	bool isDead() const;

private:
	int health;
protected:
	float deltaTime;
	Vector3 targetPos;

	Stats stats;

	Vector3 vecForward;
	Selector* root;
	BT bt;
};