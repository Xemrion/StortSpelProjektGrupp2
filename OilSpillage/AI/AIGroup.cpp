#include "AIGroup.h"

AIGroup::AIGroup()
{
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


