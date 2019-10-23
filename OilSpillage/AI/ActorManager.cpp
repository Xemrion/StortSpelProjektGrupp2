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
		if (!actors.at(i)->isDead())
		{
			actors.at(i)->run(actors, dt, targetPos);
		}
		else
		{
			actors.at(i)->death();
			delete actors.at(i);
			actors.erase(actors.begin() + i);
		}

	}
	if (frameCount % 60 == 0)
	{
		updateGroups();
		assignPathsToGroups(targetPos);
		frameCount = 0;
	}
	updateAveragePos();
	frameCount++;
}

void ActorManager::createDefender(float x, float z, Vector3 objectivePos)
{
	this->actors.emplace_back(new Defender(x, z, this->aStar, objectivePos));
	initGroupForActor(actors.at(actors.size() - 1));
}

void ActorManager::createAttacker(float x, float z)
{
	this->actors.emplace_back(new Attacker(x, z, this->aStar));
	initGroupForActor(actors.at(actors.size()-1));
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
					Vector3 curAveragePos = averagePos.at(activeGroup);
					Vector3 actorPos = actors.at(j)->getPosition();
					float deltaX = actorPos.x - curAveragePos.x;
					float deltaZ = actorPos.z - curAveragePos.z;
					float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
					bool isInRange = distance <= groupRadius;
					if (isInRange)
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

std::vector<Actor*>* ActorManager::findClosestGroup(Vector3 position)
{
	int groupNr = 0;
	int smallestDistance = 1000;

	for(int i = 0; i < this->groups.size(); i++)
	{
		float deltaX = position.x - this->averagePos.at(i).x;
		float deltaZ = position.z - this->averagePos.at(i).z;
		float distance = sqrt((deltaX * deltaX) + (deltaZ * deltaZ));
		if(distance < smallestDistance)
		{
			smallestDistance = distance;
			groupNr = i;
		}
	}
	return &groups.at(groupNr);
}

void ActorManager::spawnDefenders(std::vector<Vector3> objectives)
{
	for(int i = 0; i < objectives.size(); i++)
	{
		createDefender(objectives.at(i).x + 2, objectives.at(i).z, objectives.at(i));
		createDefender(objectives.at(i).x, objectives.at(i).z - 2, objectives.at(i));
		createDefender(objectives.at(i).x, objectives.at(i).z + 2, objectives.at(i));
		createDefender(objectives.at(i).x+2, objectives.at(i).z + 2, objectives.at(i));
		createDefender(objectives.at(i).x-2, objectives.at(i).z + 2, objectives.at(i));
	}
}

void ActorManager::spawnAttackers(Vector3 playerPos)
{
	//createAttacker(playerPos.x+20, playerPos.z);
	//createAttacker(playerPos.x+10, playerPos.z-10);
	//createAttacker(playerPos.x+10, playerPos.z-20);

	
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
		float deltaX = actorPos.x - curAveragePos.x;
		float deltaZ = actorPos.z - curAveragePos.z;
		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		bool isInRange = distance <= groupRadius;
		bool isBiggerGroup = groups.at(i).size() >= biggestGroupSize;
		if (isInRange && isBiggerGroup)
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
			if (current->isDead())
			{
				leaveGroup(i, k);
			}
			else
			{
				Vector3 curAveragePos = averagePos.at(i);
				Vector3 actorPos = current->getPosition();
				float deltaX = actorPos.x - curAveragePos.x;
				float deltaZ = actorPos.z - curAveragePos.z;
				float distanceToOwnGroup = (deltaX * deltaX) + (deltaZ * deltaZ);
				bool isInRange = distanceToOwnGroup <= groupRadius;

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

		}
		if (groups.at(i).empty())
		{
			groups.erase(groups.begin() + i);
			averagePos.erase(averagePos.begin() + i);
			i = 0;
		}
	}
}

void ActorManager::initGroupForActor(Actor* actor)
{
	//Is there a group nearby? (Join biggest)
	int groupIndex = groupInRange(actor->getPosition(),1);
	//else, create new group, join that group
	if (groupIndex != -1)
	{
		joinGroup(actor, groupIndex);
	}
	else
	{
		createGroup(actor);
	}
}

void ActorManager::createGroup(Actor* actor)
{
	groups.push_back(std::vector<Actor*>());
	groups.at(groups.size() - 1).push_back(actor);
	actor->joinGroup();
	averagePos.push_back(actor->getPosition());
}
