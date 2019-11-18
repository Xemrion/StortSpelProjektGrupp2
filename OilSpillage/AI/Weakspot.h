#pragma once
#include "DynamicActor.h"

class Weakspot : public DynamicActor
{
public:
	Weakspot();
	Weakspot(float x, float z, int weapontype, Physics* physics);
	~Weakspot();
	void update(float dt, btVector3 velocityVec);
	void startPos(Vector3 startPos);
	void shortMove(Vector3 posVec);

private:
	void setUpActor();
	void move(btVector3 velocityVec);
};