#include "ActorManager.h"
#include "../game.h"
#include "../States/PlayingGameState.h"
#include "Attacker.h"
#include "Swarm.h"
#include "Car_Spitfire.h"

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
	std::vector<Vector3> temp;
	for (int i = 0; i < this->groups.size(); i++)
	{
		temp = static_cast<PlayingGameState*>(Game::getCurrentState())->map->getTileMap().getAllTilePositionsInRadius(groups[i].getAveragePos(), 300, Tile::building);
		for(int j = 0; j < groups[i].actors.size(); j++)
		{
			if (!groups[i].actors[j]->isDead() && groups[i].actors[j] != nullptr)
			{
				groups[i].actors[j]->update(dt, targetPos);
			}
			else if (groups[i].actors[j]->isDead() && groups[i].actors[j] != nullptr)
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
	}
	if (hasDied)
	{
		for (int i = this->actors.size() - 1; i >= 0; i--)
		{
			if (actors[i]->isDead())
			{
				delete actors[i];
				actors.erase(actors.begin() + i);
			}
		}
	}
	updateGroups();
	turretHandler.update(dt, targetPos);
	if (frameCount % 20 == 0)
	{
		assignPathsToGroups(targetPos);
		frameCount = 0;
	}
	updateAveragePos();
	frameCount++;
}

void ActorManager::createAttacker(float x, float z, int weaponType)
{
	this->actors.push_back(new Attacker(x, z, weaponType));
	initGroupForActor(actors.at(actors.size() - 1));
}

void ActorManager::createTurret(float x, float z, int weaponType)
{
	turretHandler.createTurret(x, z, weaponType);
}

void ActorManager::createSpitFire(float x, float z, Physics* physics)
{
	this->actors.push_back(new Spitfire(x, z,physics));
	initGroupForActor(actors.at(actors.size() - 1));
}

void ActorManager::createSwarm(float x, float z, int weaponType)
{
	this->actors.push_back(new Swarm(x, z, weaponType));
	initGroupForActor(actors.at(actors.size() - 1));
}

float ActorManager::distanceToPlayer(Vector3 position)
{
	float minDistance = 99999999999;
	for (int i = 0; i < this->actors.size(); i++)
	{
		float distance = (position - this->actors[i]->getPosition()).Length();
		if (minDistance > distance) {
			minDistance = distance;
		}
	}
	float turretMinDist = turretHandler.distanceToPlayer(position);
	if (turretMinDist < minDistance)
	{
		return turretMinDist;
	}
	else
	{
		return minDistance;
	}
}

void ActorManager::intersectPlayerBullets(Bullet* bulletArray, size_t size)
{
	for (int i = 0; i < this->actors.size(); i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (!this->actors[i]->isDead())
			{
				if (bulletArray[j].getGameObject()->getAABB().intersectXZ(this->actors[i]->getAABB()))
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
	turretHandler.intersectPlayerBullets(bulletArray, size, soundTimer);

}

void ActorManager::spawnAttackers(Vector3 originPos, int weaponType)
{
	for (int i = 0; i < 2; i++)
	{
		createAttacker(originPos.x + i, originPos.z, weaponType);
		createAttacker(originPos.x, originPos.z + 1, weaponType);
		createAttacker(originPos.x - i, originPos.z, weaponType);
	}
}

void ActorManager::spawnTurrets(Vector3 position, Radius radius, float angle, int weaponType)
{
	if (angle != 0)
	{
		Vector2& newPosition = generateAroundaPoint(position.x, position.z, angle);
		createTurret(newPosition.x, newPosition.y, weaponType);
	}
	else
	{
		Vector2& newPosition = this->generateRandom(position.x, position.z, radius);
		createTurret(newPosition.x, newPosition.y, weaponType);
	}
}

Vector2& ActorManager::generateRandom(const float& x, const float& z, Radius radius)
{
	/*blocksize 10, 4* 10 w, 3 * 10 l */
	Vector2 newPosition;

	if (radius == Radius::CLOSE)
	{// max + min ( from min to max )
		newPosition.x = rand() % 10 + (x + 2);
		newPosition.y = rand() % 10 + (z + 2);
	}
	else if (radius == Radius::MIDDLE)
	{
		newPosition.x = rand() % 40 + (x + 2 + 10);
		newPosition.y = rand() % 40 + (z + 2 + 10);
	}
	else if (radius == Radius::OUTER)
	{
		newPosition.x = rand() % 60 + (x + 40);
		newPosition.y = rand() % 60 + (z + 40);
	}
	return newPosition;

}

Vector2& ActorManager::generateAroundaPoint(const float& x, const float& z, float angle)
{

	float radians = angle * (3.14f / 180.f);
	Vector2 newPosition;
	newPosition.x = x + (cos(radians) * x + sin(radians) * z);
	newPosition.y = z + (cos(radians) * x - sin(radians) * z);
	return newPosition;
}

void ActorManager::updateAveragePos()
{
	Vector3 totalPos;
	for (int i = 0; i < groups.size(); i++)
	{
		groups[i].updateAveragePos();
	}
}

int ActorManager::groupInRange(Vector3 actorPos, int currentGroupSize)
{
	int biggestGroupSize = currentGroupSize;
	int returnIndex = -1;
	for (int i = 0; i < groups.size(); i++)
	{
		Vector3 curAveragePos = groups[i].getAveragePos();
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

void ActorManager::joinGroup(DynamicActor* actor, int groupIndex)
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
	for (int i = 0; i < groups.size(); i++)
	{
		aStar->algorithm(groups[i].getAveragePos(), targetPos, groups[i].getPathRef());

		for (int j = 0; j < groups[i].actors.size(); j++)
		{
			groups[i].actors[j]->setPath(&groups[i].getPathRef());
		}
	}
}

void ActorManager::updateGroups()
{
	for (int i = 0; i < groups.size(); i++)
	{
		for (int k = 0; k < groups[i].actors.size(); k++)
		{
			DynamicActor* current = groups[i].actors[k];
			if (current->isDead() or current == nullptr)
			{
				leaveGroup(i, k);
			}
			else
			{
				Vector3 curAveragePos = groups[i].getAveragePos();
				Vector3 actorPos = current->getPosition();
				float deltaX = actorPos.x - curAveragePos.x;
				float deltaZ = actorPos.z - curAveragePos.z;
				float distanceToOwnGroup = (deltaX * deltaX) + (deltaZ * deltaZ);
				bool isInRange = distanceToOwnGroup <= groups[i].getGroupRadius();

				//Actor is outside its own groupRadius, check for other groups or create its own
				if (distanceToOwnGroup > groups[i].getGroupRadius())
				{
					int groupIndex = groupInRange(current->getPosition(), 0);
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

void ActorManager::initGroupForActor(DynamicActor* actor)
{
	//Is there a group nearby? (Join biggest)
	int groupIndex = groupInRange(actor->getPosition(), 0);
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

void ActorManager::createGroup(DynamicActor* actor)
{
	AIGroup temp;
	temp.actors.push_back(actor);
	temp.updateAveragePos();
	actor->joinGroup();
	groups.push_back(temp);
}