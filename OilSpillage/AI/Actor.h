#pragma once
#include "Behaviour.h"
#include "Boid.h"
#include "AStar.h"
#include "../Weapon.h"
#include "../VehicleStats.h"

class Actor : public GameObject
{
public:
	Actor();
	Actor(float x, float z, AStar* aStar);
	virtual ~Actor();
	virtual void update(float dt, Vector3 targetPos);
	virtual void setUpActor() = 0 {};

	void applyForce(Vector3 force);
	// one Laws that boids follow
	Vector3 separation(vector<Actor*>& boids, Vector3 targetPos = Vector3(0.0f, -100.0f, 0.0f));
	// Other function for moving and interacting
	Vector3 seek();
	void run(vector<Actor*>& boids, float deltaTime, Vector3 targetPos = Vector3(0.0f, -100.0f, 0.0f));
	virtual void updateBoid(float deltaTime);
	void setPath(std::vector<Vector3>* path);
	virtual bool hasGroup();
	void joinGroup();

	const int& getHealthRef() const;
	int getHealth() const;
	int getMaxHealth() const;
	void setHealth(int health);
	void setMaxHealth(int maxHealth);
	void resetHealth();
	void changeHealth(int amount);
	bool isDead() const;
	void death();
	std::vector<Vector3>* path;

private:
	Vector3 acceleration;
	float maxSpeed;
	float maxForce;
	bool isInGroup = false;
	int attackRange = 8;
protected:
	bool predicting = false;
	float deltaTime;
	Vector3 velocity;
	int health;
	Stats defaultStats;
	Stats updatedStats;

	Vector3 vecForward;
	Vector3 destination;
	Selector* root;
	AStar* aStar;
	Vector3 targetNode;
	enum State { Roaming, Chasing, Returning };
	State state;
	BT bt;
	Vector3 targetPos;

	virtual Status shoot();
	virtual Status inAttackRange();	
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