#pragma once
#include"Objective.h"
class ObjectiveHandler
{
public:
	ObjectiveHandler();
	~ObjectiveHandler();
	void addObjective(TypeOfMission type, int rewardTime, int nrOfTargets, std::string info, GameObject** target = nullptr, Actor** enemies = nullptr);
	bool isAllDone();
	void update(Vector3 playerPos);
	void reset();
	Objective* getObjective(int id)const;
private:
	std::vector<Objective*> objectiveVec;
	

};

