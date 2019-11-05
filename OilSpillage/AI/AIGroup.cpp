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
	int boidOffset = (rand() % 20);
	for(int i = 0; i < actors.size(); i++)
	{
		averagePos += actors[i]->getPosition();
		actors[i]->boidOffset = boidOffset;
	}
	averagePos /= actors.size();
}

std::vector<Vector3>& AIGroup::getPathRef()
{
	return this->path;
}

void AIGroup::updateBoidDistance()
{
	int boidOffset = (rand() % 20);
	for(int i = 0; i < actors.size(); i++)
	{
		actors[i]->boidOffset = boidOffset;
	}
}

