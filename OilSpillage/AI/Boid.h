#pragma once
//#include "GameObject.h"
#include "..//GameObject.h"
#include <vector>

using namespace std;

class Boid : public GameObject
{
private:
	Vector3 location;
	Vector3 velocity;
	Vector3 acceleration;
	float maxSpeed;
	float maxForce;

	Vector3 targetPosition;

public:
	Boid() {}
	Boid(float x, float z);
	void applyForce(Vector3 force);
	// Three Laws that boids follow
	Vector3 separation(vector<Boid*> boids);
	Vector3 alignment(vector<Boid*> boids);
	Vector3 cohesion(vector<Boid*> boids);
	//Functions involving SFML and visualisation linking
	Vector3 seek(Vector3 target);
	void run(vector<Boid*> boids, float deltaTime);
	void update(float deltaTime);
	void flock(vector<Boid*> boids);
	float angle(Vector3 target);
	Vector3 getLocation();
};

