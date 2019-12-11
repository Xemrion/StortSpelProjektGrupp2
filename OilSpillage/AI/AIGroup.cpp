#include "AIGroup.h"
#include <algorithm>

AIGroup::AIGroup()
{
	groupRadius = 6.5f * 6.5f;
}

AIGroup::~AIGroup()
{
}

void AIGroup::update()
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

void AIGroup::setPath(std::vector<Vector3> path)
{
	this->path = path;
}

void AIGroup::removeActor(DynamicActor* actor)
{
	for (int i = 0; i < actors.size(); i++)
	{
		if (actors[i] == actor)
		{
			actors.erase(actors.begin() + i);
			break;
		}
	}
}

void AIGroup::updateDuty()
{
	for (int i = 0; i < actors.size(); i++)
	{
		actors[i]->duty = i % 2;
	}
}

const float AIGroup::getGroupRadius() const
{
	return this->groupRadius;
}

void AIGroup::removeDeadActors()
{
	actors.erase( std::remove_if( actors.begin(),
		                           actors.end(),
		                           []( DynamicActor const *e ) {
										if (e == nullptr)
										{
											return true;
										}
										else
										{
											return e->isDead();
										}
		                           } ),
		           actors.end() );
}
