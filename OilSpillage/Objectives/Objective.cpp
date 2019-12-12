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
	this->closestIndex = 0;
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

void Objective::setSpotLight(SpotLight* theLight)
{
	this->mission->aLight = theLight;
}

void Objective::setEnemies(int nrOfEnemies)
{
	started = true;
	this->nrOfMax = nrOfEnemies;
	this->nrOfTargets = nrOfEnemies;
}

void Objective::setBoss(Boss* boss)
{
	this->mission->boss = boss;
}

void Objective::setType(TypeOfMission type)
{
	this->mission->typeMission = type;
}

void Objective::setGeneralPosition(Vector3 pos)
{
	this->mission->generalPosition = pos;
	this->closestToPlayer = pos;
}

void Objective::setTargetType(TypeOfTarget targetType)
{
	this->mission->typeOfTarget = targetType;
}

void Objective::setReward(int time)
{
	this->mission->rewardTime = time;
}

void Objective::setScore(int score)
{
	this->mission->points = score;
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

Boss* Objective::getBoss() const
{
	return this->mission->boss;
}

int Objective::getRewardTime() const
{
	return this->mission->rewardTime;
}

int Objective::getScore() const
{
	return this->mission->points;
}

GameObject* Objective::getClosestObj() const
{
	return this->mission->target[this->closestIndex];
}

std::string Objective::getInfo() const
{
	if (this->mission->typeMission == TypeOfMission::GetToPoint)
	{
		return this->mission->info;
	}
	return this->mission->info + ": " + std::to_string(this->nrOfMax - this->nrOfTargets) + " / " + std::to_string(this->nrOfMax) + " ";
}

Vector3 Objective::getAveragePosition() const
{
	Vector3 avPos = Vector3(0, 0, 0);
	if (this->mission->typeMission != TypeOfMission::KillingSpree)
	{

	}
	return avPos;
}

Vector3 Objective::getClosestToPlayer() const
{
	return this->closestToPlayer;
}

Vector3 Objective::getGeneralPosition() const
{
	return this->mission->generalPosition;
}

bool Objective::isDone()
{
	if (this->mission->typeMission == TypeOfMission::FindAndCollect || this->mission->typeMission == TypeOfMission::GetToPoint)
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
	else if (this->mission->typeMission == TypeOfMission::BossEvent)
	{
		if (this->mission->boss->getHealth()<=0.0f)
		{
			return true;
		}
	}
	return false;
}

void Objective::update(Vector3 playerPosition)
{
	PlayingGameState* ptrState = static_cast<PlayingGameState*>(Game::getCurrentState());
	Vector3 findClosestPlayer(0,0,0);
	
	if (this->mission->typeMission == TypeOfMission::FindAndCollect)
	{
		if (started)
		{
			if (this->mission->target[0] != nullptr)
			{
				this->closestIndex = 0;
				findClosestPlayer = this->mission->target[0]->getPosition();
			}
			//collision check
			int nrOfDone = 0;
			for (int i = 0; i < nrOfMax; i++)
			{

				if (this->mission->target[i] != nullptr)
				{

					Vector3 closestTemp = this->mission->target[i]->getPosition();
					
					if (i != 0 && (findClosestPlayer -playerPosition).Length() > (closestTemp-playerPosition).Length())
					{
						findClosestPlayer = closestTemp;
						this->closestIndex = i;
					}
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
				static_cast<PlayingGameState*>(Game::getCurrentState())->actorManager->createTurret(GOptr->getPosition().x + 3, GOptr->getPosition().z, 1);
				static_cast<PlayingGameState*>(Game::getCurrentState())->actorManager->createTurret(GOptr->getPosition().x, GOptr->getPosition().z + 3, 1);
				static_cast<PlayingGameState*>(Game::getCurrentState())->actorManager->createTurret(GOptr->getPosition().x - 3, GOptr->getPosition().z, 1);
			}
		}
	}
	else if (this->mission->typeMission == TypeOfMission::GetToPoint)
	{
		if (!started)
		{
			started = true;
			this->mission->aLight->setLuminance(10.0f);
		}
		else
		{
			if ((this->mission->generalPosition - playerPosition).Length() < 5.0f)
			{
				this->done = true;
			}
		}
		
	}
	if (this->mission->typeMission == TypeOfMission::GetToPoint)
	{
		this->closestToPlayer = this->mission->generalPosition;
	}
	else if(this->mission->typeMission == TypeOfMission::FindAndCollect)
	{
		this->closestToPlayer = findClosestPlayer;
		
		Vector3 pos = this->closestToPlayer;
		Vector3 towardsPlayer = playerPosition - pos;
		if (towardsPlayer.Length() <= 5.5)
		{
			//move closer to player
			towardsPlayer.Normalize();
			if (this->mission->target[this->closestIndex] != nullptr)
			{
				this->mission->target[this->closestIndex]->setPosition(this->mission->target[this->closestIndex]->getPosition() + (towardsPlayer * 0.33));
			}
		}
		else
		{
			//dont move
		}
				
	}
	else if (this->mission->typeMission == TypeOfMission::BossEvent)
	{
		this->closestToPlayer = this->mission->boss->getPosition();
	}
}

int Objective::getNrOfMax() const
{
	return this->nrOfMax;
}
