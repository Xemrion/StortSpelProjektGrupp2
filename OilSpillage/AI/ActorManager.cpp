#include "ActorManager.h"
#include "../game.h"
#include "../States/PlayingGameState.h"
ActorManager::ActorManager()
{
	this->aStar = nullptr;
}

ActorManager::ActorManager(AStar* aStar)
{
	this->aStar = aStar;
}

ActorManager::~ActorManager()
{
	for (int i = 0; i < actors.size(); i++)
	{
		delete actors[i];
	}
	actors.clear();
}

void ActorManager::update(float dt, Vector3 targetPos)
{
	soundTimer += dt;
	bool hasDied = false;
	for (int i = 0; i < this->actors.size(); i++)
	{
		if (!actors[i]->isDead() && actors[i] != nullptr)
		{
			actors[i]->run(actors, dt, targetPos);
		}
		else if (actors[i]->isDead() && actors[i] != nullptr)
		{
			Objective* ptr = static_cast<PlayingGameState*>(Game::getCurrentState())->getObjHandler().getObjective(0);
			if (ptr != nullptr)
			{
				if (ptr->getType() == TypeOfMission::KillingSpree)
				{
					ptr->killEnemy();
				}
			}
			hasDied = true;
		}
	}
	if(hasDied == true)
	{
		for (int i = this->actors.size() - 1; i >= 0; i--)
		{
			if (actors[i]->isDead())
			{
				actors[i]->death();
				delete actors[i];
				actors.erase(actors.begin() + i);
			}
		}
		hasDied = false;
	}

	updateGroups();
	if (frameCount % 60 == 0)
	{
		assignPathsToGroups(targetPos);
		frameCount = 0;
	}
	updateAveragePos();
	frameCount++;
}

void ActorManager::createDefender(float x, float z, Vector3 objectivePos)
{
	this->actors.push_back(new Defender(x, z, this->aStar, objectivePos));
	initGroupForActor(actors.at(actors.size() - 1));
}

void ActorManager::createAttacker(float x, float z)
{
	this->actors.push_back(new Attacker(x, z, this->aStar));
	initGroupForActor(actors.at(actors.size()-1));
}

void ActorManager::createTurret(float x, float z)
{
	this->actors.push_back(new Turret(x, z));
}

std::vector<Actor*>* ActorManager::findClosestGroup(Vector3 position)
{
	int rangeOfPlayer = 10*10;
	sendToPlayer.clear();

	for(int i = 0; i < this->actors.size(); i++)
	{
		float deltaX = position.x - this->actors[i]->getPosition().x;
		float deltaZ = position.z - this->actors[i]->getPosition().z;
		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		if(distance < rangeOfPlayer)
		{
			sendToPlayer.push_back(actors[i]);
		}
	}
	return &sendToPlayer;
}

float ActorManager::distanceToPlayer(Vector3 position)
{
	float minDistance=99999999999;
	for (int i = 0; i < this->actors.size(); i++)
	{
		float distance = (position - this->actors.at(i)->getPosition()).Length();
		if (minDistance > distance) {
			minDistance = distance;
		}
	}
	return minDistance;
}

void ActorManager::intersectPlayerBullets(Bullet* bulletArray, size_t size)
{
	for (int i = 0; i < this->actors.size(); i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (!this->actors[i]->isDead())
			{
				if (bulletArray[j].getGameObject()->getAABB().intersect(this->actors[i]->getAABB()))
				{
					if (soundTimer > 0.05f) {
						/*int randomSound = rand() % 3 + 1;
						std::wstring soundEffect = L"data/sound/MetalImpactPitched" + to_wstring(randomSound) + L".wav";
						Sound::PlaySoundEffect(soundEffect);*/
						Sound::PlaySoundEffect(L"data/sound/HitSound.wav");
						soundTimer = 0;
					}
					this->actors[i]->changeHealth(-bulletArray[j].getDamage());
					bulletArray[j].destroy();
				}
			}
		}
	}
}

void ActorManager::spawnDefenders(Vector3 objective)
{
	createDefender(objective.x + 2, objective.z, objective);
	createDefender(objective.x, objective.z - 2, objective);
	createDefender(objective.x, objective.z + 2, objective);
	createDefender(objective.x+2, objective.z + 2, objective);
	createDefender(objective.x-2, objective.z + 2, objective);
}

void ActorManager::spawnAttackers(Vector3 originPos)
{
	for(int i = 0; i < 2; i++)
	{
		createAttacker(originPos.x+i, originPos.z);
		createAttacker(originPos.x, originPos.z+1);
		createAttacker(originPos.x-i, originPos.z);
	}
}

void ActorManager::spawnTurrets(Vector3 playerPos)
{
	//createAttacker(playerPos.x+20, playerPos.z);
	//createAttacker(playerPos.x+10, playerPos.z-10);
	//createAttacker(playerPos.x+10, playerPos.z-20);

	
}

void ActorManager::updateAveragePos()
{
	Vector3 totalPos;
	for (int i = 0; i < groups.size(); i++)
	{
		groups[i].updateAvaragePos();
	}
}

int ActorManager::groupInRange(Vector3 actorPos, int currentGroupSize)
{
	int biggestGroupSize = currentGroupSize;
	int returnIndex = -1;
	for (int i = 0; i < groups.size(); i++)
	{
		Vector3 curAveragePos = groups[i].getAvaragePos();
		float deltaX = actorPos.x - curAveragePos.x;
		float deltaZ = actorPos.z - curAveragePos.z;
		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		bool isInRange = distance <= groups[i].getGroupRadius();
		bool isBiggerGroup = groups[i].actors.size() >= biggestGroupSize;
		if (isInRange && isBiggerGroup)
		{
			biggestGroupSize = groups[i].actors.size();
			returnIndex = i;
		}
	}
	return returnIndex;
}

void ActorManager::joinGroup(Actor* actor, int groupIndex)
{
	actor->joinGroup();
	groups.at(groupIndex).actors.push_back(actor);
}

void ActorManager::leaveGroup(int groupIndex, int where)
{
	groups[groupIndex].actors.erase(groups[groupIndex].actors.begin() + where);
}

void ActorManager::assignPathsToGroups(Vector3 targetPos)
{
	path.clear();
	for (int i = 0; i < groups.size(); i++)
	{
		aStar->algorithm(groups[i].getAvaragePos(), targetPos, path);

		for (int j = 0; j < groups[i].actors.size(); j++)
		{
			groups[i].actors[j]->setPath(path);
		}
	}
}

void ActorManager::updateGroups()
{
	for (int i = 0; i < groups.size(); i++)
	{
		for (int k = 0; k < groups[i].actors.size(); k++)
		{
			Actor* current = groups[i].actors[k];
			if (current->isDead() or current == nullptr)
			{
				leaveGroup(i, k);
			}
			else
			{
				Vector3 curAveragePos = groups[i].getAvaragePos();
				Vector3 actorPos = current->getPosition();
				float deltaX = actorPos.x - curAveragePos.x;
				float deltaZ = actorPos.z - curAveragePos.z;
				float distanceToOwnGroup = (deltaX * deltaX) + (deltaZ * deltaZ);
				bool isInRange = distanceToOwnGroup <= groups[i].getGroupRadius();

				//Actor is outside its own groupRadius, check for other groups or create its own
				if (distanceToOwnGroup > groups[i].getGroupRadius())
				{
					int groupIndex = groupInRange(current->getPosition(), groups[i].actors.size());
					//Found a group within the groupRadius
					if (groupIndex != -1)
					{
						leaveGroup(i, k);
						joinGroup(current, groupIndex);
					}
					//create its own group
					else
					{
						leaveGroup(i, k);
						createGroup(current);
					}
				}
				//If actor is inside its own group radius but is also inside another one that has more actors(encourage bigger groups)
				else
				{
					int groupIndex = groupInRange(current->getPosition(), groups[i].actors.size());

					if (groupIndex != -1 && groupIndex != i)
					{
						leaveGroup(i, k);
						joinGroup(current, groupIndex);
					}
				}
			}

		}
		if (groups[i].actors.empty())
		{
			groups.erase(groups.begin() + i);
			i = 0;
		}
	}
}

void ActorManager::initGroupForActor(Actor* actor)
{
	//Is there a group nearby? (Join biggest)
	int groupIndex = groupInRange(actor->getPosition(),1);
	//else, create new group, join that group
	if (groupIndex != -1)
	{
		joinGroup(actor, groupIndex);
	}
	else
	{
		createGroup(actor);
	}
}

void ActorManager::createGroup(Actor* actor)
{
	AIGroup temp;
	temp.actors.push_back(actor);
	temp.updateAvaragePos();
	actor->joinGroup();
	groups.push_back(temp);
}