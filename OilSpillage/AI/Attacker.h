#pragma once
#include"Ranged.h"
#include"Dynamic.h"
class Attacker : public Dynamic, public Ranged
{
public:
	Attacker();
	Attacker(float x, float z, int weaponType);
	void update(float dt, Vector3 targetPos);

	~Attacker();
private:
	void setUpActor();

};