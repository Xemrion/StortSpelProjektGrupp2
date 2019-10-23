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
	this->actors.push_back(new Defender(x, z, this->aStar, objectivePos));
	initGroupForActor(actors.at(actors.size() - 1));
}

void ActorManager::createAttacker(float x, float z)
{
	this->actors.push_back(new Attacker(x, z, this->aStar));
	initGroupForActor(actors.at(actors.size()-1));
}

void ActorManager::createTurret(float x, float z)
{
	this->actors.push_back(new Turret(x, z));
}
std::vector<Actor*>* ActorManager::findClosestGroup(Vector3 position)
{
	int rangeOfPlayer = 10*10;
	sendToPlayer.clear();

	for(int i = 0; i < this->actors.size(); i++)
	{
		float deltaX = position.x - this->actors.at(i)->getPosition().x;
		float deltaZ = position.z - this->actors.at(i)->getPosition().z;
		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		if(distance < rangeOfPlayer)
		{
			sendToPlayer.push_back(actors.at(i));
		}
	}
	return &sendToPlayer;
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

		averagePos.push_back(totalPos / groups.at(i).size());
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
