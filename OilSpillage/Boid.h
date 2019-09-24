#pragma once
#include "GameObject.h"
#include <vector>

using namespace std;

class Boid : public GameObject
{
private:
	bool predator;
	Vector3 location;
	Vector3 velocity;
	Vector3 acceleration;
	float maxSpeed;
	float maxForce;

	Vector3 desiredPosition;

public:
	Boid() {}
	Boid(float x, float z);
	void applyForce(Vector3 force);
	// Three Laws that boids follow
	Vector3 Separation(vector<Boid*> Boids);
	Vector3 Alignment(vector<Boid*> Boids);
	Vector3 Cohesion(vector<Boid*> Boids);
	//Functions involving SFML and visualisation linking
	Vector3 seek(Vector3 target);
	void run(vector<Boid*> Boids, float deltaTime);
	void update(float deltaTime);
	void flock(vector<Boid*> Boids);
	float angle(Vector3 target);
	Vector3 getLocation();
};

