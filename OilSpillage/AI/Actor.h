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

	//void run(vector<Actor*>& boids, float deltaTime, vector<Vector3> buildings, Vector3 targetPos = Vector3(0.0f, -100.0f, 0.0f));

	virtual bool hasGroup();
	void joinGroup();

	int getHealth() const;
	void setHealth(int health);
	void changeHealth(int amount);
	bool isDead() const;

private:
	bool isInGroup = false;
	int health;
protected:
	Stats defaultStats;
	Stats updatedStats;
	float deltaTime;
	Vector3 targetPos;



	Vector3 vecForward;
	Selector* root;
	BT bt;



};