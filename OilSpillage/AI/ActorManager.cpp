#include "ActorManager.h"

ActorManager::ActorManager()
{
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
		actors.at(i)->update(dt, targetPos);
		actors.at(i)->run(actors, dt);
	}
}

void ActorManager::createDefender(float x, float z)
{
	this->actors.emplace_back(new Defender(x, z, this->aStar));
}

void ActorManager::createAttacker(float x, float z)
{
	this->actors.emplace_back(new Attacker(x, z, this->aStar));
}

void ActorManager::setAStar(AStar* aStar)
{
	this->aStar = aStar;
}

void ActorManager::initGroups()
{
	for (int i = 0; i < this->actors.size(); i++)
	{
		if (actors.at(i)->getGroupNR() == -1)
		{
			//Create new group
			groups.push_back(std::vector<Actor*>());
			int activeGroup = groups.size() - 1;

			groups.at(activeGroup).push_back(actors.at(i));
			actors.at(i)->joinGroup(activeGroup);
			averagePos.push_back(actors.at(i)->getPosition());
			for (int j = 0; j < actors.size(); j++)
			{
				if (actors.at(j)->getGroupNR() == -1)
				{
					if ((actors.at(j)->getPosition() - averagePos.at(activeGroup)).Length() < 2)
					{
						groups.at(activeGroup).push_back(actors.at(j));
						actors.at(j)->joinGroup(activeGroup);
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

void ActorManager::updateGroups()
{
	for (int i = 0; i < groups.size(); i++)
	{
		for (int k = 0; k < groups.at(i).size(); k++)
		{
			float distanceToOwnGroup = (groups.at(i).at(k)->getPosition() - averagePos.at(groups.at(i).at(k)->getGroupNR())).Length();
			for (int j = 0; j < groups.size(); j++)
			{
				if (distanceToOwnGroup > (groups.at(i).at(k)->getPosition() - averagePos.at(j)).Length())
				{
					groups.at(i).at(k)->joinGroup(j);
					groups.at(j).push_back(groups.at(i).at(k));
					groups.at(i).erase(groups.at(i).begin() + k);
				}
			}
		}
	}
}
