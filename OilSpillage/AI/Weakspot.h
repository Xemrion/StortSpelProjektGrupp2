#pragma once
#include "../game.h"
#include "../GameObject.h"
#include "../VehicleStats.h"

class Weakspot : public GameObject
{
public:
	//operators
	Weakspot();
	Weakspot(Weakspot&& weakspot);
	Weakspot(int weapontype);
	~Weakspot();
	Weakspot& operator=(const Weakspot& other);

	void startPos(Vector3 startPos);
	void shortMove(Vector3 posVec);

	//gets and sets
	void setWeakspotNr(int weakspotNr);
	const bool getDead();
	const int getWeakspotNr();

private:
	int weakspotNr;
	Stats stats;
	int health;
};