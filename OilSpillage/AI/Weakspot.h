#pragma once
#include "../game.h"
#include "../GameObject.h"
#include "../VehicleStats.h"

class Weakspot : public GameObject
{
public:
	Weakspot();
	Weakspot(int weapontype, Stats stats = VehicleStats::AIPart);
	~Weakspot();
	void update(float dt, btVector3 velocityVec);
	void startPos(Vector3 startPos);
	void shortMove(Vector3 posVec);
	void updateSelf();

	void setWeakspotNr(int weakspotNr);

	const bool getDead();
	const int getWeakspotNr();

private:
	bool dead;
	int weakspotNr;
	Stats stats;

	void setUpActor();
	void move(btVector3 velocityVec);
};