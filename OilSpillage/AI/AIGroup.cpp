#include "AIGroup.h"

AIGroup::AIGroup()
{
	groupRadius = 6.5f * 6.5f;
}

AIGroup::~AIGroup()
{
}

void AIGroup::update(const Vector3& targetPos)
{
	removeDeadActors();
	updateAveragePos();
	formation(targetPos);
}

const Vector3& AIGroup::getAveragePos() const
{
	return this->averagePos;
}

void AIGroup::updateAveragePos()
{
	averagePos = Vector3(0.0f);
	if (actors.size() > 0)
	{
		for (int i = 0; i < actors.size(); i++)
		{
			averagePos += actors[i]->getPosition();
		}
		averagePos /= actors.size();

	}
}

std::vector<Vector3>* AIGroup::getPathPtr()
{
	return &this->path;
}

void AIGroup::setPath(std::vector<Vector3> path)
{
	this->path = path;
}
void AIGroup::formation(const Vector3& targetPos)
{
	float deltaX = averagePos.x - targetPos.x;
	float deltaZ = averagePos.z - targetPos.z;
	float distance = (deltaX * deltaX) + (deltaZ * deltaZ);

	if (distance < 70 && distance > 3)
	{
		this->groupRadius = expandedGroupRadius;
		for (int i = 0; i < actors.size(); i++)
		{
			actors[i]->vActive = true;
			actors[i]->groupPos = averagePos;
		}
	}
	else
	{
		this->groupRadius = defaultGroupRadius;
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

void AIGroup::removeDeadActors()
{
	for (int i = actors.size() - 1; i >= 0; i--)
	{
		if (actors[i]->isDead())
		{
			actors.erase(actors.begin() + i);
		}
	}
}
