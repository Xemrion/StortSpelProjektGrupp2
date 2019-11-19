#pragma once
#include "DynamicActor.h"
#include "BossAbilities.h"
#include "Weakspot.h"

class Boss : public DynamicActor, public BossAblilities
{
public:
	Boss();
	Boss(Boss&& boss);
	Boss(float x, float z, int weaponType, Physics* physics); //, std::vector<Weakspot*>
	virtual ~Boss();
	void update(float dt, const Vector3& targetPos);
	void checkIfWeakPointHit(Bullet* bulletArray, size_t size, float soundTimer);

private:
	float timeTilNextPoint;
	float timeTilRotationChange;
	int timeRotateDelay;
	int currentPointNr;
	int rotationVar; //which way to rotate
	int phase;
	Vector3 currentPoint;
	Vector3 playerPos;
	std::vector<Weakspot> weakSpots; //need to be pointers?
	Vector3 frontVector;
	btVector3 currentVelocityVector;

	void setUpActor();
	void movementVariables(float dt);
	//Behaiours
	void move();
	Vector3 seek();
	void circulatePlayer(Vector3 targetPos);
	void enterPhase2();

	void initiateWeakPoints();
	void updateWeakPoints();
};