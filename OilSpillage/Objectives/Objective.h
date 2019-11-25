#pragma once
#include"..///GameObject.h"
#include"..///AI/Actor.h"
enum class TypeOfTarget
{
	Box,
	Reactor,
	Crate,
	Scrap,
	Size
};
enum class TypeOfMission
{
	SearchAndDestroy,
	KillingSpree,
	FindAndCollect
};
enum class Reward
{
	Time,
	Progress
};
struct Mission
{
	Actor* *enemies;
	GameObject* *target;//target or targets
	TypeOfMission typeMission;
	TypeOfTarget typeOfTarget;
	int rewardTime;
	int points;
	std::string info;
};
class Objective
{
public:
	Objective();
	~Objective();
	void setTarget(GameObject* *target, int nrOfTargets);
	void setEnemies(Actor* *enemies, int nrOfEnemies);
	void setType(TypeOfMission type);
	void setTargetType(TypeOfTarget targetType);
	void setReward(int time);
	void setScore(int score);
	void setInfo(std::string info);
	void killEnemy();
	GameObject* getTarget(int id)const;
	TypeOfMission getType()const;
	TypeOfTarget getTargetType()const;

	int getRewardTime()const;
	int getScore()const;
	std::string getInfo()const;
	Vector3 getAveragePosition()const;
	Vector3 getClosestToPlayer()const;
	bool isDone();
	void update(Vector3 playerPosition);
	int getNrOfMax()const;
private:
	Mission* mission;
	Vector3 closestToPlayer;
	bool done;
	bool started;
	int nrOfMax;
	int nrOfTargets;
};
