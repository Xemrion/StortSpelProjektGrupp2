#pragma once
#include "Behaviour.h"
#include "Boid.h"
#include "AStar.h"

class Actor : public GameObject
{
public:
	Actor();
	Actor(float x, float z);
	~Actor();
	virtual void update(float dt, Vector3 targetPos);
	void setAStar(AStar* aStar);
	virtual void setUpActor() = 0 {};

	void applyForce(Vector3 force);
	// Three Laws that boids follow
	Vector3 separation(vector<Actor*> boids);
	Vector3 alignment(vector<Actor*> boids);
	Vector3 cohesion(vector<Actor*> boids);
	// Other function for moving and interacting
	Vector3 seek(Vector3 target);
	void run(vector<Actor*> boids, float deltaTime);
	void updateBoid(float deltaTime);
	void flock(vector<Actor*> boids);
	float angle(Vector3 target);
	Vector3 getDestination();
	void setDestination(Vector3 destination);
private:
	int nrOfFrames = 0;
	Vector3 velocity;
	Vector3 acceleration;
	float maxSpeed;
	float maxForce;

	Vector3 destination;
protected:
	std::vector<Node*> path;
	AStar* aStar;
	Vector3 targetNode;
	enum State { Roaming, Chasing, Returning };
	State state;

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
	virtual void  followPath();
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