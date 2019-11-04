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

