#pragma once
#include"Objective.h"

struct Type
{
	std::string types[int(TypeOfTarget::Size)];
	Vector4 colors[int(TypeOfTarget::Size)];
	std::string texture[int(TypeOfTarget::Size)];
	std::string getType(TypeOfTarget type)
	{
		return types[int(type)];
	};
	Vector4 getColor(TypeOfTarget type)
	{
		return colors[int(type)];
	};
	std::string getTexture(TypeOfTarget type)
	{
		return texture[int(type)];
	};
};
class ObjectiveHandler
{
public:
	ObjectiveHandler();
	~ObjectiveHandler();
	void addObjective(TypeOfMission type, int rewardTime, int nrOfTargets, std::string info, TypeOfTarget targetType=TypeOfTarget::Size, GameObject** target = nullptr, Actor** enemies = nullptr);
	bool isAllDone();
	void update(Vector3 playerPos);
	Type getTypes()const;
	void reset();
	Objective* getObjective(int id)const;
	bool& getEventNewObj();
private:
	Type types;
	std::vector<Objective*> objectiveVec;
	std::vector<GameObject**> pickUpArrs;
	std::vector<int> nrOfPickupsVec;
	bool eventNewObj;

};

