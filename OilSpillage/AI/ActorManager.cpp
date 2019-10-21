#include "ActorManager.h"

ActorManager::ActorManager()
{
	this->aStar = nullptr;
}

ActorManager::ActorManager(AStar* aStar)
{
	this->aStar = aStar;
}

ActorManager::~ActorManager()
{
	for (int i = 0; i < actors.size(); i++)
	{
		delete actors.at(i);
	}
	actors.clear();
}

void ActorManager::update(float dt, Vector3 targetPos)
{
	for (int i = 0; i < this->actors.size(); i++)
	{
		actors.at(i)->run(actors, dt, targetPos);
	}
	updateGroups();
	updateAveragePos();
	//assignPathsToGroups(targetPos);
}

void ActorManager::createDefender(float x, float z, Vector3 objectivePos)
{
	this->actors.emplace_back(new Defender(x, z, this->aStar, objectivePos));
}

void ActorManager::createAttacker(float x, float z)
{
	this->actors.emplace_back(new Attacker(x, z, this->aStar));
}

void ActorManager::createTurret(float x, float z)
{
	this->actors.emplace_back(new Turret(x, z));
}

void ActorManager::initGroups()
{
	for (int i = 0; i < this->actors.size(); i++)
	{
		if (!actors.at(i)->hasGroup())
		{
			//Create new group
			createGroup(actors.at(i));
			int activeGroup = groups.size() - 1;
			for (int j = 0; j < actors.size(); j++)
			{
				if (!actors.at(j)->hasGroup())
				{
					if ((actors.at(j)->getPosition() - averagePos.at(activeGroup)).Length() < groupRadius)
					{
						groups.at(activeGroup).push_back(actors.at(j));
						actors.at(j)->joinGroup();
						Vector3 totalPos;
						for (int k = 0; k < groups.at(activeGroup).size(); k++)
						{
							totalPos += groups.at(activeGroup).at(k)->getPosition();
						}
						averagePos.erase(averagePos.begin() + activeGroup);
						averagePos.insert(averagePos.begin() + activeGroup, totalPos / groups.at(activeGroup).size());
					}
				}
			}
		}
	}
}

void ActorManager::updateAveragePos()
{
	Vector3 totalPos;
	averagePos.clear();
	for (int i = 0; i < groups.size(); i++)
	{
		for (int j = 0; j < groups.at(i).size(); j++)
		{
			totalPos += groups.at(i).at(j)->getPosition();
		}

		averagePos.emplace_back(totalPos / groups.at(i).size());
	}
}

int ActorManager::groupInRange(Vector3 actorPos, int currentGroupSize)
{
	int biggestGroupSize = currentGroupSize;
	int returnIndex = -1;
	for (int i = 0; i < groups.size(); i++)
	{
		Vector3 curAveragePos = averagePos.at(i);
		float deltaX = actorPos.x -curAveragePos.x;
		float deltaZ = actorPos.z - curAveragePos.z;
		float distance = sqrt((deltaX* deltaX) + (deltaZ * deltaZ));
		if (distance <= groupRadius &&
			groups.at(i).size() >= biggestGroupSize)
		{
			biggestGroupSize = groups.at(i).size();
			returnIndex = i;
		}

	}
	return returnIndex;
}

void ActorManager::joinGroup(Actor* actor, int groupIndex)
{
	actor->joinGroup();
	groups.at(groupIndex).push_back(actor);
}

void ActorManager::leaveGroup(int groupIndex, int where)
{
	groups.at(groupIndex).erase(groups.at(groupIndex).begin() + where);

}

void ActorManager::assignPathsToGroups(Vector3 targetPos)
{
	std::vector<Vector3> path;
	for (int i = 0; i < groups.size(); i++)
	{
		aStar->algorithm(averagePos.at(i), targetPos, path);

		for (int j = 0; j < groups.at(i).size(); j++)
		{
			groups.at(i).at(j)->setPath(path);
		}

	}
}

void ActorManager::updateGroups()
{
	for (int i = 0; i < groups.size(); i++)
	{
		for (int k = 0; k < groups.at(i).size(); k++)
		{
			Actor* current = groups.at(i).at(k);
			float distanceToOwnGroup = (current->getPosition() - averagePos.at(i)).Length();

			//Actor is outside its own groupRadius, check for other groups or create its own
			if (distanceToOwnGroup > groupRadius)
			{
				int groupIndex = groupInRange(current->getPosition(), groups.at(i).size());
				//Found a group within the groupRadius
				if (groupIndex != -1)
				{
					leaveGroup(i, k);
					joinGroup(current, groupIndex);
				}
				//create its own group
				else
				{
					leaveGroup(i, k);
					createGroup(current);
				}

			}
			//If actor is inside its own group radius but is also inside another one that has more actors(encourage bigger groups)
			else
			{
				int groupIndex = groupInRange(current->getPosition(), groups.at(i).size());
			
				if (groupIndex != -1 && groupIndex != i)
				{
					leaveGroup(i, k);
					joinGroup(current, groupIndex);
				}
			}
		}
		if (groups.at(i).empty())
		{
			groups.erase(groups.begin() + i);
			averagePos.erase(averagePos.begin() + i);
			i = 0;
		}
	}
}

void ActorManager::createGroup(Actor* actor)
{
	groups.push_back(std::vector<Actor*>());
	groups.at(groups.size() - 1).push_back(actor);
	actor->joinGroup();
	averagePos.push_back(actor->getPosition());
}
