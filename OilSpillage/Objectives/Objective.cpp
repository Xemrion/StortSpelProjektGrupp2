#include"Objective.h"
#include"..///game.h"
Objective::Objective()
{
	this->mission = new Mission;
	this->done = false;
	this->started = false;
	this->nrOfMax = 0;
	this->nrOfTargets = 0;
}

Objective::~Objective()
{
	if (this->mission->typeMission==TypeOfMission::FindAndCollect)
	{
		delete[] this->mission->target;
	}
	if (!(this->mission->typeMission==TypeOfMission::FindAndCollect))
	{
		//delete[] this->mission->enemies;
	}
	delete this->mission;
}



void Objective::setTarget(GameObject* *target, int nrOfTargets)
{
	started = true;
	mission->target = new GameObject*[nrOfTargets];
	for (int i = 0; i < nrOfTargets; i++)
	{
		this->mission->target[i] = target[i];
	}
	this->nrOfMax = nrOfTargets;
	this->nrOfTargets = nrOfTargets;
}

void Objective::setEnemies(Actor* *enemies, int nrOfEnemies)
{
	started = true;
	/*mission->enemies = new Actor*[nrOfEnemies];
	for (int i = 0; i < nrOfEnemies; i++)
	{
		this->mission->target[i] = enemies[i];
	}*/
	this->nrOfMax = nrOfEnemies;
	this->nrOfTargets = nrOfEnemies;
}

void Objective::setType(TypeOfMission type)
{
	this->mission->typeMission = type;
}

void Objective::setReward(int time)
{
	this->mission->rewardTime = time;
}

void Objective::setInfo(std::string info)
{
	this->mission->info = info;
}

void Objective::killEnemy()
{
	//int found = -1;
	//for (int i = 0; i < nrOfTargets && found==-1; i++)
	//{
	//	if (this->mission->enemies[i] == enemy)
	//	{
	//		found = i;
	//	}
	//}
	//Actor* temp = this->mission->enemies[nrOfTargets - 1];
	//this->mission->enemies[found] = temp;//
	//this->mission->enemies[nrOfTargets - 1] = nullptr;


	this->nrOfTargets--;
}

GameObject* Objective::getTarget(int id) const
{
	return this->mission->target[id];
}

TypeOfMission Objective::getType() const
{
	return this->mission->typeMission;
}

int Objective::getRewardTime() const
{
	return this->mission->rewardTime;
}

std::string Objective::getInfo() const
{
	return this->mission->info + " " + std::to_string(this->nrOfMax - this->nrOfTargets) + " / " + std::to_string(this->nrOfMax) + " ";
}

Vector3 Objective::getAveragePosition() const
{
	Vector3 avPos = Vector3(0, 0, 0);
	/*for (int i = 0; i < nrOfTargets; i++)
	{
		avPos += this->mission->enemies[i]->getPosition();
	} 
	avPos /= float(nrOfTargets);*/
	return avPos;
}

bool Objective::isDone()
{
	if (this->mission->typeMission == TypeOfMission::FindAndCollect)
	{
		return done;
	}
	else if (this->mission->typeMission == TypeOfMission::KillingSpree)
	{
		if (started)
		{
			if (this->nrOfTargets<=0)
			{
				return true;
			}
		}
	}
	else if (this->mission->typeMission == TypeOfMission::SearchAndDestroy)
	{
		if (this->mission->target[0] == nullptr)
		{
			return true;
		}
	}
	return false;
}

void Objective::update(Vector3 playerPosition)
{
	if (this->mission->typeMission == TypeOfMission::FindAndCollect)
	{
		//collision check
		int nrOfDone = 0;
		for (int i = 0; i < nrOfMax; i++)
		{
			if (this->mission->target[i] != nullptr)
			{
				Vector3 vecPlayerToObj = playerPosition - this->mission->target[i]->getPosition();
				if (vecPlayerToObj.Length() < this->mission->target[i]->getScale().x*2.0f)
				{
					this->mission->target[i]->setPosition(Vector3(1000, 1000, 1000));
					GameObject* temp = this->mission->target[nrOfMax -1];
					this->mission->target[i] = temp;//
					this->mission->target[nrOfMax -1] = nullptr;
					this->nrOfTargets--;
				}
			}
			else
			{
				nrOfDone++;
			}
		}
		if (nrOfDone == nrOfMax)
		{
			done = true;
		}
	}
}

int Objective::getNrOfMax() const
{
	return this->nrOfMax;
}
