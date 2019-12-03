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
	Weakspot(int weapontype, float scalingNr);
	Weakspot(const Weakspot& other);
	~Weakspot();
	Weakspot& operator=(const Weakspot& other);

	void startPos(Vector3 startPos);
	void shortMove(Vector3 posVec);
	void changeHealth(int damage, float dt);

	//gets and sets
	void setWeakspotNr(int weakspotNr);
	void setIsHit(bool isHit);

	const bool getDead();
	const int getWeakspotNr();
	const int getHealth();
	const int getMaxHP();
	const bool getIsHit();

private:
	float scalingNr;
	int weakspotNr;
	Stats stats;
	int health;
	int maxHP;
	bool isHit;
};