#include "AIGroup.h"

AIGroup::AIGroup()
{
	groupRadius = 6.5f * 6.5f;
}

AIGroup::~AIGroup()
{
}

const Vector3& AIGroup::getAveragePos() const
{
	return this->averagePos;
}

void AIGroup::updateAveragePos()
{
	averagePos = Vector3(0.0f);
	for(int i = 0; i < actors.size(); i++)
	{
		averagePos += actors[i]->getPosition();
	}
	averagePos /= actors.size();
}

std::vector<Vector3>* AIGroup::getPathPtr()
{
	return &this->path;
}

void AIGroup::setPath(std::vector<Vector3> path)
{
	this->path = path;
}
void AIGroup::formationP(const Vector3& targetPos)
{
	float deltaX = averagePos.x - targetPos.x;
	float deltaZ = averagePos.z - targetPos.z;
	float distance = (deltaX * deltaX) + (deltaZ * deltaZ);

	if (distance < 50 && distance > 2)
	{
		this->groupRadius = groupRadius * 2;
		for (int i = 0; i < actors.size(); i++)
		{
			actors[i]->vActive = true;
			actors[i]->groupPos = averagePos;
		}
	}
	else
	{
		this->groupRadius = 6.5f * 6.5f;
	}
}

void AIGroup::updateDuty()
{
	for (int i = 0; i < actors.size(); i++)
	{
		if (i < actors.size() / 2)
		{
			actors[i]->duty = 1;
		}
		else
		{
			actors[i]->duty = 2;
		}
	}
}

const float AIGroup::getGroupRadius() const
{
	return this->groupRadius;
}
