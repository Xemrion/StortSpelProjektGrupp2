#include "ChaseCar.h"

void ChaseCar::followPath()
{
	if (path != nullptr)
	{
		if (path->size() > 0 && (position - targetPos).Length() > 40)
		{
			destination = path->at(path->size() - 1);
			if (position.Distance(path->at(path->size() - 1), position) < 2)
			{
				path->pop_back();
			}
		}
		else
		{
			destination = targetPos;
		}
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
