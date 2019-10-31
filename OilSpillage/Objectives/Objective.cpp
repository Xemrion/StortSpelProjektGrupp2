#include"Objective.h"
#include"..///game.h"
#include"..//States/PlayingGameState.h"
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
	//started = true;
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
	this->nrOfMax = nrOfEnemies;
	this->nrOfTargets = nrOfEnemies;
}

void Objective::setType(TypeOfMission type)
{
	this->mission->typeMission = type;
}

void Objective::setTargetType(TypeOfTarget targetType)
{
	this->mission->typeOfTarget = targetType;
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

TypeOfTarget Objective::getTargetType() const
{
	return this->mission->typeOfTarget;
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
	if (this->mission->typeMission != TypeOfMission::KillingSpree)
	{

	}
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
	PlayingGameState* ptrState = static_cast<PlayingGameState*>(Game::getCurrentState());

	if (this->mission->typeMission == TypeOfMission::FindAndCollect)
	{
		if (started)
		{
			//collision check
			int nrOfDone = 0;
			for (int i = 0; i < nrOfMax; i++)
			{
				if (this->mission->target[i] != nullptr)
				{
					float time = ptrState->getTime();
					float sine = sin((time-0) / 0.3f) - 0.5f;
					float color = max(sine, 0.0f);
					
					Vector4 combineColor(color, color, color, 1.0);
					combineColor += ptrState->getObjHandler().getTypes().getColor(this->mission->typeOfTarget);
					this->mission->target[i]->setColor(combineColor);

					Vector3 vecPlayerToObj = playerPosition - this->mission->target[i]->getPosition();
					if (vecPlayerToObj.Length() < this->mission->target[i]->getScale().x * 2.0f)
					{
						this->mission->target[i]->setPosition(Vector3(1000, 1000, 1000));
						GameObject* temp = this->mission->target[nrOfMax - 1];
						this->mission->target[i] = temp;//
						this->mission->target[nrOfMax - 1] = nullptr;
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
		else
		{
			started = true;
			GameObject* GOptr;
			Vector2 randOrigin(rand() % 1000, -(rand() % 1000));
			randOrigin += Vector2(playerPosition.x, playerPosition.y);
			for (int i = 0; i < nrOfMax; i++)
			{
				GOptr = this->mission->target[i];
				float yPos = GOptr->getPosition().y;
				Vector3 randPos(0, 0, 0);
				float distance = 0.0f;

				randPos = ptrState->generateObjectivePos(Vector3(randOrigin.x, 0.0f, randOrigin.y), 40, 1000.0f);

				GOptr->setPosition(Vector3(randPos.x,yPos,randPos.z));
				Game::getGraphics().addToDraw(GOptr);
				static_cast<PlayingGameState*>(Game::getCurrentState())->actorManager->spawnDefenders(GOptr->getPosition());
				//static_cast<PlayingGameState*>(Game::getCurrentState())->actorManager->spawnTurrets(GOptr->getPosition(), ActorManager::Radius::CLOSE, 0);
				//static_cast<PlayingGameState*>(Game::getCurrentState())->actorManager->spawnTurrets(GOptr->getPosition(), ActorManager::Radius::CLOSE, 50);
			}
		}
	}
}

int Objective::getNrOfMax() const
{
	return this->nrOfMax;
}
