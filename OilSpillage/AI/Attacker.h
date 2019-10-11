#pragma once
#include "Actor.h"
class Attacker : public Actor
{
public:
	Attacker();
	Attacker(float x, float z, AStar* aStar);
	~Attacker();
private:
	void setUpActor();

};