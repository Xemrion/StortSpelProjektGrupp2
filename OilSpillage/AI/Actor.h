#pragma once
#include "Behaviour.h"
#include "Boid.h"
#include "AStar.h"
#include "../Weapon.h"

class Actor : public GameObject
{
public:
	Actor();
	Actor(float x, float z, AStar* aStar);
	~Actor();
	virtual void update(float dt, Vector3 targetPos);
	virtual void setUpActor() = 0 {};

	void applyForce(Vector3 force);
	// Three Laws that boids follow
	Vector3 separation(vector<Actor*> boids, Vector3 targetPos = Vector3(0.0f, -100.0f, 0.0f));
	Vector3 alignment(vector<Actor*> boids);
	Vector3 cohesion(vector<Actor*> boids);
	// Other function for moving and interacting
	Vector3 seek(Vector3 target);
	void run(vector<Actor*> boids, float deltaTime, Vector3 targetPos = Vector3(0.0f, -100.0f, 0.0f));
	virtual void updateBoid(float deltaTime);
	void flock(vector<Actor*> boids, Vector3 targetPos = Vector3(0.0f, -100.0f, 0.0f));
	float angle(Vector3 target);
	void setPath(std::vector<Node*> path);
	Vector3 getDestination();
	bool hasGroup();
	void setDestination(Vector3 destination);
	void joinGroup();
private:
	Vector3 acceleration;
	float maxSpeed;
	float maxForce;
	bool isInGroup = false;
protected:
	Vector3 velocity;
	int nrOfFrames = 0;
	Vector3 destination;
	Selector* root;
	std::vector<Node*> path;
	AStar* aStar;
	Vector3 targetNode;
	enum State { Roaming, Chasing, Returning };
	State state;
	BT bt;
	float deltaTime;
	Vector3 targetPos;

	void findPath();
	void chase();
	void roam();
	virtual Status shoot();
	virtual Status inRange();
	virtual Status enemyNear();
	virtual Status setChaseState();
	virtual Status setRoamState();
	virtual void  followPath();
	virtual void  updateWeapon(float deltaTime);

	Weapon weapon;
	static const int bulletCount = 32;
	float timeSinceLastShot;
	Bullet bullets[bulletCount];
};