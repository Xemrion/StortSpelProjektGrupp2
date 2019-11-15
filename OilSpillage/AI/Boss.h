#pragma once
#include "DynamicActor.h"
#include "BossAbilities.h"
#include "Weakspot.h"

class Boss : public DynamicActor, public BossAblilities
{
public:
	Boss();
	Boss(float x, float z, int weaponType, std::vector<Weakspot*>);
	virtual ~Boss();
	void update(float dt, const Vector3& targetPos);
private:
	float timeTilNextPoint;
	float timeTilRotationChange;
	int rotationVar; //which way to rotate
	int currentPointNr;
	Vector3 currentPoint;
	Vector3 playerPos;
	std::vector<Weakspot*> weakSpots; //How to add these to actormanager
	Vector3 frontVector;

	void setUpActor();
	void movementVariables(float dt);
	//Behaiours
	void move();
	Vector3 seek();
	void circulatePlayer(Vector3 targetPos);
	void updateWeakPoints(Vector3 targetPos);
	void saveWeakspots(std::vector<Weakspot*> weakspots);
};
