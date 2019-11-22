#include "ActorManager.h"
#include "../game.h"
#include "../States/PlayingGameState.h"
#include "Attacker.h"
#include "Swarm.h"
#include "ChaseCar.h"
#include "ShootCar.h"
#include "Boss.h"
#include "Sniper.h"

ActorManager::ActorManager()
{
	this->aStar = nullptr;
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
}

void ActorManager::update(float dt, const Vector3& targetPos)
{
	soundTimer += dt;
	spawnTimer -= dt;
	//seperation(targetPos);
	updateActors(dt, targetPos);

	if (spawnTimer <= 0)
	{
		spawnEnemies(targetPos);
		spawnTimer = spawnCooldown;
	}

	Vector3 newPos;
	float deltaX;
	float deltaZ;
	float distance;
	for (int i = 0; i < groups.size(); i++)
	{
		groups[i].update(targetPos);
		deltaX = groups[i].averagePos.x - targetPos.x;
		deltaZ = groups[i].averagePos.z - targetPos.z;
		distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		//(TileSize * nrOfTiles)^2
		if (distance > (20 * 5) * (20 * 5))
		{
			newPos = generateObjectivePos(targetPos, 50, 75);
			for (int j = 0; j < groups[i].actors.size(); j++)
			{
				groups[i].actors[j]->setPosition(Vector3(newPos.x, groups[i].actors[j]->getPosition().y, newPos.z));
				physics->teleportRigidbody(Vector3(newPos.x, groups[i].actors[j]->getPosition().y, newPos.z), groups[i].actors[j]->getRigidBody());
				if (j % 5 == 0)
				{
					newPos = generateObjectivePos(targetPos, 50, 100);
				}
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
	frameCount++;
}

void ActorManager::createAttacker(float x, float z, int weaponType)
{
	this->actors.push_back(new Attacker(x, z, weaponType, physics));
	initGroupForActor(actors.at(actors.size() - 1));
}

void ActorManager::createSniper(float x, float z, int weaponType)
{
	this->actors.push_back(new Sniper(x, z, weaponType, physics));
	initGroupForActor(actors.at(actors.size() - 1));
}

void ActorManager::createTurret(float x, float z, int weaponType)
{
	turretHandler.createTurret(x, z, weaponType, physics);
}

void ActorManager::createSpitFire(float x, float z)
{
	this->actors.push_back(new Spitfire(x, z, physics));
	initGroupForActor(actors.at(actors.size() - 1));
}

void ActorManager::createChaseCar(float x, float z)
{
	this->actors.push_back(new ChaseCar(x, z, physics));
	initGroupForActor(actors.at(actors.size() - 1));
}

void ActorManager::createShootCar(float x, float z, int weaponType)
{
	this->actors.push_back(new ShootCar(x, z, weaponType, physics));
	initGroupForActor(actors.at(actors.size() - 1));
}

void ActorManager::createSwarm(float x, float z)
{
	this->actors.push_back(new Swarm(x, z, physics));
	initGroupForActor(actors.at(actors.size() - 1));
}

void ActorManager::createBoss(float x, float z, int weaponType)
{
	this->actors.push_back(new Boss(x, z, weaponType, physics));
	initGroupForActor(actors.at(actors.size() - 1));
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

const std::vector<AIGroup>& ActorManager::getGroups() const
{
	return this->groups;
}
//Player vs AI
void ActorManager::intersectPlayerBullets(Bullet* bulletArray, size_t size)
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
					if (this->actors[i]->getAABB().intersectXZ(rayOrigin, rayDir, laserObject->getScale().z * 2))
					{
						if (soundTimer > 0.05f) {
							Sound::play("./data/sound/HitSound.wav");
							soundTimer = 0;
						}
						this->actors[i]->changeHealth(-bulletArray[j].getDamage());
					}
				}
				else if (bulletArray[j].getTimeLeft() > 0 && bulletArray[j].getGameObject()->getAABB().intersectXZ(this->actors[i]->getAABB()))
				{
					if (soundTimer > 0.05f) {
						Sound::play("./data/sound/HitSound.wav");
						soundTimer = 0;
					}
					this->actors[i]->changeHealth(-bulletArray[j].getDamage());
					bulletArray[j].destroy();
				}
				if (bulletArray[j].getMelee() && bulletArray[j].getGameObject()->getAABB().intersectXZ(this->actors[i]->getAABB()))
				{
					if (soundTimer > 0.05f) {
						Sound::play("data/sound/HitSound.wav");
						soundTimer = 0;
					}
					this->actors[i]->changeHealth(-bulletArray[j].getDamage());
					// dont remove the melee weapon
				}
			}
		}
	}
	turretHandler.intersectPlayerBullets(bulletArray, size, soundTimer);

}

void ActorManager::spawnAttackers(const Vector3& originPos)
{
	for (int i = 0; i < 2; i++)
	{
		createAttacker(originPos.x + i, originPos.z, (rand() % 4) + 1);
		createAttacker(originPos.x, originPos.z + i, (rand() % 4) + 1);
		createAttacker(originPos.x - i, originPos.z, (rand() % 4) + 1);
	}
}
void ActorManager::spawnSnipers(const Vector3& originPos)
{
	for (int i = 0; i < 2; i++)
	{
		createSniper(originPos.x + i, originPos.z, (rand() % 4) + 1);
		createSniper(originPos.x, originPos.z + i, (rand() % 4) + 1);
		createSniper(originPos.x - i, originPos.z, (rand() % 4) + 1);
	}
}
void ActorManager::spawnChaseCars(const Vector3& originPos)
{
	for (int i = 0; i < 2; i++)
	{
		createChaseCar(originPos.x + i, originPos.z);
		createChaseCar(originPos.x, originPos.z + i);
		createChaseCar(originPos.x - i, originPos.z);
	}
}

void ActorManager::spawnShootCars(const Vector3& originPos)
{
	for (int i = 0; i < 2; i++)
	{
		createShootCar(originPos.x + i, originPos.z, (rand() % 4) + 1);
		createShootCar(originPos.x, originPos.z + i, (rand() % 4) + 1);
		createShootCar(originPos.x - i, originPos.z, (rand() % 4) + 1);
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

void ActorManager::spawnTurrets(const Vector3& position, Radius radius, float angle)
{
	if (angle != 0)
	{
		Vector2& newPosition = generateAroundaPoint(position.x, position.z, angle);
		createTurret(newPosition.x, newPosition.y, 1);
	}
	else
	{
		Vector2& newPosition = this->generateRandom(position.x, position.z, radius);
		createTurret(newPosition.x, newPosition.y, 1);
	}
}

void ActorManager::spawnBoss(const Vector3& originPos, int weaponType)
{
	//createBoss(originPos.x, originPos.z, weaponType);
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

void ActorManager::seperation(const Vector3& targetPos)
{
	std::vector<Vector3> buildings;
	float desiredSeparationDistance;
	for (int i = 0; i < groups.size(); i++)
	{
		for (int j = 0; j < groups[i].actors.size(); j++)
		{
			// Distance of field of vision for separation between boids
			desiredSeparationDistance = groups[i].actors[j]->getBoidOffset();
			Vector3 direction(0.0f);
			float nrInProximity = 0.0f;
			// For every boid in the system, check if it's too close
			for (int k = 0; k < actors.size(); k++)
			{
				// Calculate distance from current boid to boid we're looking at
				Vector3 curBoidPos = actors[k]->getPosition();
				float deltaX = groups[i].actors[j]->getPosition().x - curBoidPos.x;
				float deltaZ = groups[i].actors[j]->getPosition().z - curBoidPos.z;
				float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
				// If this is a fellow boid and it's too close, move away from it
				if ((distance < desiredSeparationDistance) && distance != 0)
				{
					Vector3 difference(0.0f);
					difference = groups[i].actors[j]->getPosition() - actors[k]->getPosition();
					difference.Normalize();
					difference /= distance;      // Weight by distance
					direction += difference;
					nrInProximity++;
				}
			}
			// Adds average difference of location to acceleration
			if (nrInProximity > 0)
			{
				direction /= nrInProximity;
			}
			if (direction.Length() > 0.0f)
			{
				// Steering = Desired - Velocity
				direction.Normalize();
				direction *= groups[i].actors[j]->getMaxSpeed();
				direction -= groups[i].actors[j]->getVelocity();
				if (direction.Length() > groups[i].actors[j]->getMaxForce())
				{
					direction /= direction.Length();
				}
			}
			groups[i].actors[j]->applyForce(direction * 4);
		}
	}
}

void ActorManager::updateAveragePos()
{
	for (int i = 0; i < groups.size(); i++)
	{
		groups[i].updateAveragePos();
	}
}

void ActorManager::updateActors(float dt, Vector3 targetPos)
{
	bool hasDied = false;
	for (int i = 0; i < this->groups.size(); i++)
	{
		for (int j = 0; j < groups[i].actors.size(); j++)
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
		float normalizedRandom = float(rand()) / RAND_MAX;

		if (normalizedRandom >= 0.95)
		{
			static_cast<PlayingGameState*>(Game::getCurrentState())->addPowerUp(
				PowerUp(groups[i].actors[j]->getPosition(),
					PowerUpType::Health)
			);
		}
		for (int i = this->actors.size() - 1; i >= 0; i--)
		{
			if (actors[i]->isDead())
			{
				destroyActor(i);
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
	groups.at(groupIndex).actors.push_back(actor);
	groups[groupIndex].updateDuty();
}

void ActorManager::spawnEnemies(const Vector3& targetPos)
{
	if (actors.size() < maxNrOfEnemies)
	{
		int enemyType = rand() % 4;
		Vector3 newPos = generateObjectivePos(targetPos, 50, 100);
		if (enemyType == 0)
		{
			spawnAttackers(newPos);
		}
		else if (enemyType == 1)
		{
			spawnChaseCars(newPos);
		}
		else if (enemyType == 2)
		{
			spawnShootCars(newPos);
		}
		else if (enemyType == 3)
		{
			spawnSwarm(newPos);
		}
	}
}

void ActorManager::leaveGroup(int groupIndex, int where)
{
	groups[groupIndex].actors.erase(groups[groupIndex].actors.begin() + where);
}

void ActorManager::assignPathsToGroups(const Vector3& targetPos)
{
	std::vector<Vector3> pathToPlayer;
	std::vector<Vector3> pathToPredicted;
	std::vector<Vector3>* pathToUse;
	for (int i = 0; i < groups.size(); i++)
	{
		aStar->algorithm(groups[i].getAveragePos(), targetPos, pathToPlayer);
		aStar->algorithm(groups[i].getAveragePos(), predictPlayerPos(targetPos), pathToPredicted);

		if (pathToPlayer.size() < pathToPredicted.size())
		{
			pathToUse = &pathToPlayer;
		}
		else
		{
			pathToUse = &pathToPredicted;
		}
		groups[i].setPath(*pathToUse);
		for (int j = 0; j < groups[i].actors.size(); j++)
		{
			groups[i].actors[j]->setPath(groups[i].path.data() + groups[i].path.size() - 1);
			groups[i].actors[j]->pathSize = groups[i].path.size() - 1;
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
			if (current->isDead() || current == nullptr)
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

void ActorManager::destroyActor(int index)
{
	if (actors[index]->getRigidBody() != nullptr)
	{
		physics->DeleteRigidBody(actors[index]->getRigidBody());
	}
	delete actors[index];
	actors.erase(actors.begin() + index);
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
	groups.push_back(temp);
	groups[groups.size() - 1].updateDuty();
}

Vector3 ActorManager::predictPlayerPos(const Vector3& targetPos)
{
	Vector3 targetVelocity = Vector3(static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getVehicle()->getRigidBody()->getLinearVelocity());
	targetVelocity.Normalize();
	Vector3 predictedPos = targetPos + targetVelocity * 20;
	return predictedPos;
}
Vector3 ActorManager::generateObjectivePos(const Vector3& targetPos, float minDistance, float maxDistance) noexcept
{

	for (;;) {
		Vector3 position = map->generateRoadPositionInWorldSpace(*rng);
		float distance = (position - targetPos).Length();
		if ((distance <= maxDistance) && (distance >= minDistance))
		{
			return position;
		}
	}
	assert(false and "BUG: Shouldn't be possible!");
	return { -1.0f, -1.0f, -1.0f }; //  silences a warning

}