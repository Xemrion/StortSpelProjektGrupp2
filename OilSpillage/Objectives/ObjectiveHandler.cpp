#include"ObjectiveHandler.h"

ObjectiveHandler::ObjectiveHandler()
{
}

ObjectiveHandler::~ObjectiveHandler()
{
}

void ObjectiveHandler::addObjective(TypeOfMission tpye, int rewardTime, std::string info, GameObject** target, Actor** enemies)
{
	
}

bool ObjectiveHandler::isAllDone()
{
	bool allDone = false;
	for (Objective object : this->objectiveVec)
	{
		allDone = object.isDone();
		if (allDone == false)
		{
			break;//om den hittar minst en så är alla inte klara
		}
	}
	return allDone;
}

void ObjectiveHandler::update(Vector3 playerPos)
{
	for (Objective object : this->objectiveVec)
	{
		object.update(playerPos);
	}
}
