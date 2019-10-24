#include"ObjectiveHandler.h"

ObjectiveHandler::ObjectiveHandler()
{
}

ObjectiveHandler::~ObjectiveHandler()
{
	for (int i = 0; i < this->objectiveVec.size(); i++)
	{
		delete this->objectiveVec.at(i);
	}
	this->objectiveVec.clear();
}

void ObjectiveHandler::addObjective(TypeOfMission type, int rewardTime,int nrOfTargets, std::string info, GameObject** target, Actor** enemies)
{
	Objective *temp = new Objective;
	temp->setType(type);
	temp->setInfo(info);
	temp->setReward(rewardTime);
	if (type == TypeOfMission::FindAndCollect)
	{
		temp->setTarget(target, nrOfTargets);
	}
	else
	{
		temp->setEnemies(enemies, nrOfTargets);
	}
	this->objectiveVec.push_back(temp);
}

bool ObjectiveHandler::isAllDone()
{
	bool allDone = false;
	for (Objective* object : this->objectiveVec)
	{
		allDone = object->isDone();
		if (allDone == false)
		{
			break;//om den hittar minst en så är alla inte klara
		}
	}
	return allDone;
}

void ObjectiveHandler::update(Vector3 playerPos)
{
	for (Objective* object : this->objectiveVec)
	{
		object->update(playerPos);
	}
}

void ObjectiveHandler::reset()
{
	this->objectiveVec.clear();
}

Objective* ObjectiveHandler::getObjective(int id) const
{
	return this->objectiveVec.at(id);
}
