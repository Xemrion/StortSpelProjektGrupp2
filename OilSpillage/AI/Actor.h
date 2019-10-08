#pragma once
#include "Behaviour.h"
#include "Boid.h"
#include "AStar.h"


class Actor : public GameObject
{
public:
	Actor();
	~Actor();
	void update(float dt, Vector3 targetPos);
	void setAStar(AStar* aStar);

private:
	enum State{Roaming,Chasing};
	State state;
	void findPath();
	void shoot(float deltaTime, Vector3 targetPos);
	void chase();
	void roam();
	Status inRange();
	Status setChaseState();
	Status setRoamState();
	void  followPath();
	void setUpActor();
	Selector* root;
	BT bt;
	std::vector<Boid*> boids;

	std::vector<Node*> path;
	AStar* aStar;
	Vector3 targetNode;
	int nrOfFrames = 0;

	struct Weapon
	{
		int damage = 100;
		float fireSpeed = 3.0f;
		float bulletSpeed = 4.0f;
		float bulletLifetime = 2.0f;
	};
	Weapon weapon;

	struct Bullet
	{
		Vector3 dir;
		float speed = 0.0f;
		float timeLeft = 0.0f;
		GameObject* obj = nullptr;
	};
	static const int bulletCount = 16;
	float leftoverTime;
	Bullet bullets[bulletCount];

	float deltaTime;
	Vector3 targetPos;

};