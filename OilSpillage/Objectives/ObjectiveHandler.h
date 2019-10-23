#pragma once
#include"Objective.h"
class ObjectiveHandler
{
public:
	ObjectiveHandler();
	~ObjectiveHandler();
	void addObjective(TypeOfMission tpye, int rewardTime, std::string info, GameObject** target = nullptr, Actor** enemies = nullptr);
	bool isAllDone();
	void update(Vector3 playerPos);
private:
	std::vector<Objective> objectiveVec;
	

};

