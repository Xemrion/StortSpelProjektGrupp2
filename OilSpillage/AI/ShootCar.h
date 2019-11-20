#pragma once
#include "Car_Spitfire.h"
#include "Ranged.h"
class ShootCar : public Spitfire, public Ranged
{
private:
protected:
	virtual void followPath();
public:
	ShootCar();
	ShootCar(float x, float z, Physics* physics);
	~ShootCar();
	void update(float dt, const Vector3& targetPos);
	Status shoot();
};