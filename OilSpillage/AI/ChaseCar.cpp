#include "ChaseCar.h"

void ChaseCar::followPath()
{
	if (pathSize >= 0 && (position - targetPos).Length() > 40)
	{
		destination = *path;
		if ((destination - position).Length() < 15)
		{
			path--;
			pathSize--;
		}
	}
	else
	{
		destination = targetPos;
	}
}

ChaseCar::ChaseCar()
{
}

ChaseCar::ChaseCar(float x, float z,Physics* physics)
	:Spitfire(x, z,physics)
{
	this->stats = VehicleStats::fastCar;
	setHealth(this->stats.maxHealth);
}

ChaseCar::~ChaseCar()
{
}

void ChaseCar::update(float dt, const Vector3& targetPos)
{
	Spitfire::update(dt, targetPos);
}
