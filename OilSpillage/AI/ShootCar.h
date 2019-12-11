#pragma once
#include "Car_Spitfire.h"
#include "Ranged.h"
class ShootCar : public Spitfire, public Ranged
{
private:
	void createRigidbody(Physics* physics);
	void setUpActor();
protected:
	virtual void followPath();
public:
	ShootCar();
	ShootCar(float x, float z, int weaponType, Physics* physics);
	virtual ~ShootCar();
	void update(float dt, const Vector3& targetPos);
	Status shoot();
};