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
	int size = actors.size() - 1;
	for (int i = size; i >= 0; i--)
	{
		if (actors[i]->isDead())
		{
			actors.erase(actors.begin() + i);
		}
	}
}
