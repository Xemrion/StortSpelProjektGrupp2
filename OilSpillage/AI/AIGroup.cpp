#include "AIGroup.h"

AIGroup::AIGroup()
{
}

AIGroup::~AIGroup()
{
}

Vector3 AIGroup::getAveragePos()
{
	return this->averagePos;
}

float AIGroup::getGroupRadius()
{
	return this->groupRadius;
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

std::vector<Vector3>& AIGroup::getPathRef()
{
	return this->path;
}
void AIGroup::formationP(Vector3 targetPos)
{
	float deltaX = averagePos.x - targetPos.x;
	float deltaZ = averagePos.z - targetPos.z;
	float distance = (deltaX * deltaX) + (deltaZ * deltaZ);

	if (distance < 150 && distance > 2)
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
