#pragma once
#include "DynamicActor.h"


class Weakspot : public DynamicActor
{
public:
	Weakspot();
	Weakspot(float x, float z, int weapontype);
	~Weakspot();
	void update(float dt, Vector3 thisMovement);
	void place(Vector3 thisPos);

private:
	void setUpActor();
};