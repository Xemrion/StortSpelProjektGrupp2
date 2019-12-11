#include "ActorManager.h"
#include "../game.h"
#include "../States/PlayingGameState.h"
#include "Attacker.h"
#include "Swarm.h"
#include "ChaseCar.h"
#include "ShootCar.h"
#include "Boss.h"
#include "Sniper.h"
#define SPAWN_ENEMIES 1
ActorManager::ActorManager()
{
}

ActorManager::ActorManager(AStar* aStar, Physics* physics, Map* map, std::mt19937* RNG)
	:turretHandler(physics)
{
	this->aStar = aStar;
	this->physics = physics;
	this->map = map;
	this->rng = RNG;
}

ActorManager::~ActorManager()
{
	for (int i = 0; i < actors.size(); i++)
	{
		delete actors[i];
	}
	actors.clear();
	for (int i = 0; i < bosses.size(); i++)
	{
		delete bosses[i];
	}
	bosses.clear();
}

void ActorManager::update(float dt, const Vector3& targetPos)
{
	soundTimer += dt;
	spawnTimer -= dt;
	updateActors(dt, targetPos);
	updateBosses(dt, targetPos);

#if SPAWN_ENEMIES
	if (spawnTimer <= 0)
	{
		spawnEnemies(targetPos);

		spawnTimer = spawnCooldown;
	}
#endif


	updateGroups();
	for (int i = 0; i < groups.size(); i++)
	{
		groups[i]->update();
	}
	teleportActorsToPlayer(targetPos);
	turretHandler.update(dt, targetPos);
	if (frameCount % 20 == 0)
	{
		assignPathsToGroups(targetPos);
		frameCount = 0;
	}
	frameCount++;
}

void ActorManager::createAttacker(float x, float z, int weaponType)
{
	this->actors.push_back(new Attacker(x, z, weaponType, physics));
	initGroupForActor(actors[actors.size() - 1]);
}

void ActorManager::createTurret(float x, float z, int weaponType)
{
	turretHandler.createTurret(x, z, weaponType, physics);
}

void ActorManager::createChaseCar(float x, float z)
{
	this->actors.push_back(new ChaseCar(x, z, physics));
	initGroupForActor(actors[actors.size() - 1]);
}

void ActorManager::createShootCar(float x, float z, int weaponType)
{
	this->actors.push_back(new ShootCar(x, z, weaponType, physics));
	initGroupForActor(actors[actors.size() - 1]);
}

void ActorManager::createSwarm(float x, float z)
{
	this->actors.push_back(new Swarm(x, z, physics));
	initGroupForActor(actors[actors.size() - 1]);
}

Boss* ActorManager::createBoss(float x, float z, int weaponType, float scalingNr)
{
	//generate start pos for boss
	Vector3 newPos = findTeleportPos(Vector3(x, 0, z), 50, 100);

	Boss* boss = new Boss(newPos.x, newPos.z, weaponType, physics, scalingNr);
	this->bosses.push_back(boss);

	return boss;
}

float ActorManager::distanceToPlayer(const Vector3& position)
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
	if (turretMinDist < minDistance && turretMinDist != -1)
	{
		return turretMinDist;
	}
	else
	{
		return minDistance;
	}
}

const std::vector<AIGroup*>& ActorManager::getGroups() const
{
	return this->groups;
}
//Player vs AI
void ActorManager::intersectPlayerBullets(Bullet* bulletArray, size_t size, float deltaTime)
{
	for (int i = 0; i < this->actors.size(); i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (!this->actors[i]->isDead())
			{
				if (bulletArray[j].getWeaponType() == WeaponType::Laser)
				{
					GameObject* laserObject = bulletArray[j].getGameObject();
					Vector3 rayDir = bulletArray[j].getDirection();
					Vector3 rayOrigin = laserObject->getPosition() - rayDir * laserObject->getScale().z;
					if (this->actors[i]->getAABB().intersectXZ(rayOrigin, rayDir, laserObject->getScale().z, -1.0))
					{
						if (soundTimer > 0.05f) {
							Sound::play("HitSound.wav");
							soundTimer = 0;
						}
						if (bulletArray[j].getFlame())// Damage over Time
						{
							actors[i]->setFire(bulletArray[j].getFlameTimer());
						}
						if (bulletArray[j].getKnockback())// Knockback
						{
							actors[i]->knockBack(bulletArray[j].getDirection(), bulletArray[j].getKnockbackForce());
						}
						if (bulletArray[j].getSplashBool())
						{
							for (int k = 0; k < actors.size(); k++)
							{
								float deltaX = actors[k]->getPosition().x - bulletArray[j].getGameObject()->getPosition().x;
								float deltaZ = actors[k]->getPosition().z - bulletArray[j].getGameObject()->getPosition().z;
								float distance = sqrt((deltaX * deltaX) + (deltaZ * deltaZ));
								if (k != i && distance < bulletArray[j].getSplashRange() && !actors[k]->isDead())
								{
									actors[k]->changeHealth((-bulletArray[j].getDamage() / (20 - Game::getGameInfo().nrOfClearedStages))*deltaTime * 2);
								}
							}
						}
						this->actors[i]->changeHealth(-bulletArray[j].getDamage() * deltaTime);
					}
				}
				else if (/*bulletArray[j].getTimeLeft() > 0 && */bulletArray[j].getGameObject()->getAABB().intersectXZ(this->actors[i]->getAABB()))
				{
					if (soundTimer > 0.05f) {
						if (bulletArray[j].getWeaponType() == WeaponType::Star)
						{
							Sound::play("StarPowerupHit.mp3", 0.75f);
						}
						else
						{
							Sound::play("HitSound.wav");
						}
						soundTimer = 0;
					}
					if (bulletArray[j].getFlame())// Damage over Time
					{
						actors[i]->setFire(bulletArray[j].getFlameTimer());
					}
					if (bulletArray[j].getKnockback())// Knockback
					{
						actors[i]->knockBack(bulletArray[j].getDirection(), bulletArray[j].getKnockbackForce());
					}
					if (bulletArray[j].getSplashBool())
					{
						for (int k = 0; k < actors.size(); k++)
						{
							float deltaX = actors[k]->getPosition().x - bulletArray[j].getGameObject()->getPosition().x;
							float deltaZ = actors[k]->getPosition().z - bulletArray[j].getGameObject()->getPosition().z;
							float distance = sqrt((deltaX * deltaX) + (deltaZ * deltaZ));
							if (k != i && distance < bulletArray[j].getSplashRange() && !actors[k]->isDead())
							{
								actors[k]->changeHealth((-bulletArray[j].getDamage() / (20 - Game::getGameInfo().nrOfClearedStages)) * deltaTime * 2);
							}
						}
					}
					if(bulletArray[j].getMelee())
					{
						this->actors[i]->changeHealth(-bulletArray[j].getDamage()*deltaTime);
					}
					else
					{
						this->actors[i]->changeHealth(-bulletArray[j].getDamage());
					}
					if (!bulletArray[j].getMelee())
						bulletArray[j].destroy();
				}
			}
		}
	}
	turretHandler.intersectPlayerBullets(bulletArray, size, deltaTime, soundTimer);

	if (this->bosses.size() > 0)
	{
		for (int i = 0; i < this->bosses.size(); i++)
		{
			this->bosses[i]->checkIfWeakPointHit(bulletArray, size, soundTimer);
			for (int j = 0; j < size; j++)
			{
				if (!this->bosses[i]->isDead())
				{
					if (bulletArray[j].getWeaponType() == WeaponType::Laser)
					{
						GameObject* laserObject = bulletArray[j].getGameObject();
						Vector3 rayDir = bulletArray[j].getDirection();
						Vector3 rayOrigin = laserObject->getPosition() - rayDir * laserObject->getScale().z;
						if (this->bosses[i]->getAABB().intersectXZ(rayOrigin, rayDir, laserObject->getScale().z, -1.0))
						{
							if (soundTimer > 0.05f) {
								Sound::play("HitSound.wav");
								soundTimer = 0;
							}
							if (bulletArray[j].getFlame())// Damage over Time
							{
								bosses[i]->setFire(bulletArray[j].getFlameTimer());
							}
							if (bulletArray[j].getKnockback())// Knockback
							{
								bosses[i]->knockBack(bulletArray[j].getDirection(), bulletArray[j].getKnockbackForce());
							}
							if (bulletArray[j].getSplashBool())
							{
								for (int k = 0; k < actors.size(); k++)
								{
									float deltaX = actors[k]->getPosition().x - bulletArray[j].getGameObject()->getPosition().x;
									float deltaZ = actors[k]->getPosition().z - bulletArray[j].getGameObject()->getPosition().z;
									float distance = sqrt((deltaX * deltaX) + (deltaZ * deltaZ));
									if (k != i && distance < bulletArray[j].getSplashRange() && !actors[k]->isDead())
									{
										actors[k]->changeHealth((-bulletArray[j].getDamage() / (20 - Game::getGameInfo().nrOfClearedStages))* deltaTime * 2);
									}
								}
							}
							this->bosses[i]->changeHealth(-bulletArray[j].getDamage() * deltaTime);
						}
					}
					else if (bulletArray[j].getTimeLeft() > 0 && bulletArray[j].getGameObject()->getAABB().intersectXZ(this->bosses[i]->getAABB()))
					{
						if (soundTimer > 0.05f) {
							Sound::play("HitSound.wav");
							soundTimer = 0;
						}
						if (bulletArray[j].getFlame())// Damage over Time
						{
							bosses[i]->setFire(bulletArray[j].getFlameTimer());
						}
						if (bulletArray[j].getKnockback())// Knockback
						{
							bosses[i]->knockBack(bulletArray[j].getDirection(), bulletArray[j].getKnockbackForce());
						}
						if (bulletArray[j].getSplashBool())
						{
							for (int k = 0; k < actors.size(); k++)
							{
								float deltaX = actors[k]->getPosition().x - bulletArray[j].getGameObject()->getPosition().x;
								float deltaZ = actors[k]->getPosition().z - bulletArray[j].getGameObject()->getPosition().z;
								float distance = sqrt((deltaX * deltaX) + (deltaZ * deltaZ));
								if (k != i && distance < bulletArray[j].getSplashRange() && !actors[k]->isDead())
								{
									actors[k]->changeHealth((-bulletArray[j].getDamage() / (20 - Game::getGameInfo().nrOfClearedStages))* deltaTime * 2);
								}
							}
						}
						this->bosses[i]->changeHealth(-bulletArray[j].getDamage());
						bulletArray[j].destroy();
					}
					if (bulletArray[j].getMelee() && bulletArray[j].getGameObject()->getAABB().intersectXZ(this->bosses[i]->getAABB()))
					{
						if (soundTimer > 0.05f) {
							Sound::play("HitSound.wav");
							soundTimer = 0;
						}
						if (bulletArray[j].getFlame())// Damage over Time
						{
							bosses[i]->setFire(bulletArray[j].getFlameTimer());
						}
						if (bulletArray[j].getKnockback())// Knockback
						{
							bosses[i]->knockBack(bulletArray[j].getDirection(), bulletArray[j].getKnockbackForce());
						}
						if (bulletArray[j].getSplashBool())
						{
							for (int k = 0; k < actors.size(); k++)
							{
								float deltaX = actors[k]->getPosition().x - bulletArray[j].getGameObject()->getPosition().x;
								float deltaZ = actors[k]->getPosition().z - bulletArray[j].getGameObject()->getPosition().z;
								float distance = sqrt((deltaX * deltaX) + (deltaZ * deltaZ));
								if (k != i && distance < bulletArray[j].getSplashRange() && !actors[k]->isDead())
								{
									actors[k]->changeHealth((-bulletArray[j].getDamage() / (20 - Game::getGameInfo().nrOfClearedStages))* deltaTime * 2);
								}
							}
						}
						this->bosses[i]->changeHealth(-bulletArray[j].getDamage());
						// dont remove the melee weapon
					}
				}
			}
		}
	}

}

void ActorManager::spawnAttackers(const Vector3& originPos)
{
	for (int i = 0; i < 2; i++)
	{
		createAttacker(originPos.x + i, originPos.z, (rand() % 8) + 1);
		createAttacker(originPos.x, originPos.z + i, (rand() % 8) + 1);
		createAttacker(originPos.x - i, originPos.z, (rand() % 8) + 1);
	}
}

void ActorManager::spawnChaseCars(const Vector3& originPos)
{
	for (int i = 0; i < 2; i++)
	{
		createChaseCar(originPos.x + i, originPos.z);
	}
}

void ActorManager::spawnShootCars(const Vector3& originPos)
{
	for (int i = 0; i < 2; i++)
	{
		createShootCar(originPos.x + i, originPos.z, (rand() % 8) + 1);
	}
}

void ActorManager::spawnSwarm(const Vector3& originPos)
{
	for (int i = 0; i < 2; i++)
	{
		createSwarm(originPos.x + i, originPos.z);
		createSwarm(originPos.x, originPos.z + i);
		createSwarm(originPos.x - i, originPos.z);
	}
}

void ActorManager::teleportActorsToPlayer(const Vector3& targetPos)
{
	Vector3 newPos;
	float deltaX;
	float deltaZ;
	float distance;
	for (int i = 0; i < groups.size(); i++)
	{
		deltaX = groups[i]->averagePos.x - targetPos.x;
		deltaZ = groups[i]->averagePos.z - targetPos.z;
		distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		//(TileSize * nrOfTiles)^2
		if (distance > (20 * 10) * (20 * 10))
		{
			newPos = findTeleportPos(targetPos, 50, 100);
			for (int j = 0; j < groups[i]->actors.size(); j++)
			{
				Actor* current = groups[i]->actors[j];
				current->setGameObjectPos(Vector3(newPos.x, current->getPosition().y, newPos.z));
				//physics->teleportRigidbody(Vector3(newPos.x, current->getPosition().y, newPos.z), current->getRigidBody());
				Spitfire* ptr = dynamic_cast<Spitfire*>(current);
				if (ptr != nullptr) {
					current->getRigidBody()->getWorldTransform().setOrigin(btVector3(newPos.x, current->getPosition().y, newPos.z));
					ptr->getVehicleBody1()->getRigidBody()->getWorldTransform().setOrigin(btVector3(newPos.x, current->getPosition().y + 0.55f, newPos.z));
				}
				else {
					current->getRigidBody()->getWorldTransform().setOrigin(btVector3(newPos.x, current->getPosition().y, newPos.z));
				}

				if (j % 5 == 0)
				{
					//newPos = findTeleportPos(targetPos, 50, 100);
				}
			}
		}
	}
}

void ActorManager::updateActors(float dt, const Vector3& targetPos)
{
	for (int i = this->actors.size() - 1; i >= 0; i--)
	{
		if (actors[i]->isDead())
		{
			actorDied(i);
			continue;
		}
		actors[i]->update(dt, targetPos);
	}
}

void ActorManager::updateBosses(float dt, const Vector3& targetPos)
{
	bool hasDied = false;
	for (int i = 0; i < this->bosses.size(); i++)
	{
		if (bosses[i] != nullptr && !bosses[i]->isDead())
		{
			bosses[i]->update(dt, targetPos); //creash
		}
		else if (bosses[i] != nullptr && bosses[i]->isDead())
		{
			hasDied = true;
		}
	}

	if (hasDied)
	{
		for (int i = this->bosses.size() - 1; i >= 0; i--)
		{
			if (bosses[i]->isDead())
			{
				Game::getGameInfo().addHighScore(bosses[i]->getPoints());
				destroyBoss(i);
			}
		}
	}
}

int ActorManager::groupInRange(const Vector3& actorPos, int currentGroupSize)
{
	int biggestGroupSize = currentGroupSize;
	int returnIndex = -1;
	for (int i = 0; i < groups.size(); i++)
	{
		Vector3 curAveragePos = groups[i]->getAveragePos();
		float deltaX = actorPos.x - curAveragePos.x;
		float deltaZ = actorPos.z - curAveragePos.z;
		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		bool isInRange = distance <= groups[i]->getGroupRadius();
		bool isBiggerGroup = groups[i]->actors.size() >= biggestGroupSize;
		if (isInRange && isBiggerGroup)
		{
			biggestGroupSize = groups[i]->actors.size();
			returnIndex = i;
		}
	}
	return returnIndex;
}

void ActorManager::joinGroup(DynamicActor* actor, int groupIndex)
{
	actor->setGroup(groups[groupIndex]);
	groups[groupIndex]->actors.push_back(actor);
	groups[groupIndex]->updateDuty();
}

void ActorManager::spawnEnemies(const Vector3& targetPos)
{
	if (actors.size() < maxNrOfEnemies)
	{
		int enemyType = rand() % 100 + 1;
		Vector3 newPos = findTeleportPos(targetPos, 50, 100);
		if (enemyType < 60)
		{
			spawnAttackers(newPos);
		}
		else if (enemyType < 75)
		{
			spawnChaseCars(newPos);
		}
		else if (enemyType < 80)
		{
			spawnShootCars(newPos);
		}
		else
		{
			spawnSwarm(newPos);
		}
	}
}

void ActorManager::leaveGroup(int groupIndex, int where)
{
	groups[groupIndex]->actors.erase(groups[groupIndex]->actors.begin() + where);
}

void ActorManager::assignPathsToGroups(const Vector3& targetPos)
{
	std::vector<Vector3> pathToPlayer;
	std::vector<Vector3> pathToPredicted;
	std::vector<Vector3>* pathToUse;
	for (int i = 0; i < groups.size(); i++)
	{
		aStar->algorithm(groups[i]->getAveragePos(), targetPos, pathToPlayer);
		aStar->algorithm(groups[i]->getAveragePos(), predictPlayerPos(targetPos), pathToPredicted);

		if (pathToPlayer.size() < pathToPredicted.size())
		{
			pathToUse = &pathToPlayer;
		}
		else
		{
			pathToUse = &pathToPredicted;
		}
		groups[i]->setPath(*pathToUse);
		for (int j = 0; j < groups[i]->actors.size(); j++)
		{
			groups[i]->actors[j]->setPath(groups[i]->path.data() + groups[i]->path.size() - 1);
			groups[i]->actors[j]->pathSize = groups[i]->path.size() - 1;
		}
	}
}

void ActorManager::updateGroups()
{
	for (int i = 0; i < groups.size(); i++)
	{
		for (int k = 0; k < groups[i]->actors.size(); k++)
		{
			DynamicActor* current = groups[i]->actors[k];
			if (current != nullptr && !current->isDead())
			{
				Vector3 curAveragePos = groups[i]->getAveragePos();
				Vector3 actorPos = current->getPosition();
				float deltaX = actorPos.x - curAveragePos.x;
				float deltaZ = actorPos.z - curAveragePos.z;
				float distanceToOwnGroup = (deltaX * deltaX) + (deltaZ * deltaZ);
				bool isInRange = distanceToOwnGroup <= groups[i]->getGroupRadius();

				//Actor is outside its own groupRadius, check for other groups or create its own
				if (distanceToOwnGroup > groups[i]->getGroupRadius())
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
					int groupIndex = groupInRange(current->getPosition(), groups[i]->actors.size());

					if (groupIndex != -1 && groupIndex != i)
					{
						leaveGroup(i, k);
						joinGroup(current, groupIndex);
					}
				}
			}
			else
			{
				leaveGroup(i, k);
			}

		}

	}
}

void ActorManager::actorDied(int index)
{
	Objective* ptr = static_cast<PlayingGameState*>(Game::getCurrentState())->getObjHandler().getObjective(0);
	if (ptr != nullptr)
	{
		if (ptr->getType() == TypeOfMission::KillingSpree)
		{
			ptr->killEnemy();
		}
	}
	float normalizedRandom = float(rand()) / RAND_MAX;
	if (normalizedRandom >= 0.950)
	{
		static_cast<PlayingGameState*>(Game::getCurrentState())->addPowerUp(
			PowerUp(actors[index]->getPosition(),
				PowerUpType::Health)
		);
	}
	Game::getGameInfo().addHighScore(actors[index]->getPoints());
	if (actors[index]->getRigidBody() != nullptr)
	{
		physics->deleteRigidBody(actors[index]->getRigidBody());
	}

	if (dynamic_cast<Spitfire*>(actors[index]))
	{
		physics->deleteRigidBody(dynamic_cast<Spitfire*>(actors[index])->getVehicleBody1()->getRigidBody());
	}
	void* grpPtr = actors[index]->getGroup();
	int grpIndex = -1;
	for (int i = 0; i < groups.size(); i++)
	{
		void* temp = groups[i];
		if (temp== grpPtr)
		{
			grpIndex = i;
		}
	}	

	groups[grpIndex]->removeActor(actors[index]);
	delete actors[index];
	actors.erase(actors.begin() + index);
	if (groups[grpIndex]->actors.empty())
	{
		AIGroup* temp = groups[grpIndex];
		groups[grpIndex] = groups[groups.size() -1];
		groups[groups.size() - 1] = temp;
		groups.pop_back();
	}
}

void ActorManager::destroyBoss(int index)
{
	if (bosses[index]->getRigidBody() != nullptr)
	{
		physics->deleteRigidBody(bosses[index]->getRigidBody());
	}
	delete bosses[index];
	bosses.erase(bosses.begin() + index);

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
	AIGroup* temp = new AIGroup();
	ptr = temp;
	temp->actors.push_back(actor);
	temp->averagePos = actor->getPosition();
	groups.push_back(temp);
	groups[groups.size() - 1]->updateDuty();
	actor->setGroup(groups[groups.size() -1]);
}

Vector3 ActorManager::predictPlayerPos(const Vector3& targetPos)
{
	Vector3 targetVelocity = Vector3(static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getRigidBody()->getLinearVelocity());
	targetVelocity.Normalize();
	Vector3 predictedPos = targetPos + targetVelocity * 20;
	return predictedPos;
}
Vector3 ActorManager::findTeleportPos(const Vector3& targetPos, float minDistance, float maxDistance) noexcept
{

	for (float i = 0;; i += 1.0) {
		Vector3 position = map->generateNonBuildingPositionInWorldSpace(*rng);
		float distance = (position - targetPos).Length();
		if ((distance <= maxDistance + i) && (distance >= minDistance))
		{
			return position;
		}
		/*	else
			{
				position = map->generateGroundPositionInWorldSpace(*rng);
				float distance = (position - targetPos).Length();
				if ((distance <= maxDistance + i) && (distance >= minDistance))
				{
					return position;
				}
			}*/
	}
	assert(false and "BUG: Shouldn't be possible!");
	return { -1.0f, -1.0f, -1.0f }; //  silences a warning

}