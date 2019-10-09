#pragma once
#include "Behaviour.h"
#include "Boid.h"
#include "AStar.h"

class Actor : public GameObject
{
public:
	Actor();
	~Actor();
	virtual void update(float dt, Vector3 targetPos);
	void setAStar(AStar* aStar);
	virtual void setUpActor();

private:
	enum State{Roaming,Chasing};
	State state;
	std::vector<Boid*> boids;

	std::vector<Node*> path;
	AStar* aStar;
	Vector3 targetNode;
	int nrOfFrames = 0;

protected:

	Selector* root;
	BT bt;
	void findPath();
	void chase();
	void roam();
	virtual Status shoot();
	virtual Status inRange();
	virtual Status enemyNear();
	virtual Status setChaseState();
	virtual Status setRoamState();
	void  followPath();
	float deltaTime;
	Vector3 targetPos;

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
};