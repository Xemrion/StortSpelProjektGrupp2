#pragma once
#include "Car_Spitfire.h"
class ChaseCar : public Spitfire
{
private:
	void createRigidbody(Physics* physics);

protected:
	virtual void followPath();
public:
	ChaseCar();
	ChaseCar(float x, float z,Physics* physics);
	virtual ~ChaseCar();
	void update(float dt, const Vector3& targetPos);

};