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
	Weakspot(const Weakspot& other);
	~Weakspot();
	Weakspot& operator=(const Weakspot& other);

	void startPos(Vector3 startPos);
	void shortMove(Vector3 posVec);
	void changeHealth(int damage);

	//gets and sets
	void setWeakspotNr(int weakspotNr);
	const bool getDead();
	const int getWeakspotNr();
	const int getHealth();

private:
	int weakspotNr;
	Stats stats;
	int health;
};