#include "TurretHandler.h"
#include"..//Objectives/Objective.h"
#include"..//States/PlayingGameState.h"
void TurretHandler::destroyTurret(int index)
{
	if (turrets[index]->getRigidBody() != nullptr)
	{
		physics->DeleteRigidBody(turrets[index]->getRigidBody());
	}
	delete turrets[index];
	turrets.erase(turrets.begin() + index);
}
TurretHandler::TurretHandler()
{
}
TurretHandler::TurretHandler(Physics* physics)
{
	this->physics = physics;
}
TurretHandler::~TurretHandler()
{
	for (int i = 0; i < turrets.size(); i++)
	{
		delete turrets[i];
	}
	turrets.clear();
}
void TurretHandler::update(float dt,const Vector3& targetPos)
{
	bool turretDied = false;
	for (int i = 0; i < this->turrets.size(); i++)
	{
		if (!turrets[i]->isDead() && turrets[i] != nullptr)
		{
			turrets[i]->update(dt, targetPos);
		}
		else if (turrets[i]->isDead() && turrets[i] != nullptr)
		{
			Objective* ptr = static_cast<PlayingGameState*>(Game::getCurrentState())->getObjHandler().getObjective(0);
			if (ptr != nullptr)
			{
				if (ptr->getType() == TypeOfMission::KillingSpree)
				{
					ptr->killEnemy();
				}
			}
			turretDied = true;
		}
	}
	if (turretDied)
	{
		for (int i = this->turrets.size() - 1; i >= 0; i--)
		{
			if (turrets[i]->isDead())
			{
				Game::getGameInfo().addHighScore(turrets[i]->getPoints());
				destroyTurret(i);
			}
		}
	}
}

void TurretHandler::createTurret(float x, float z, int weaponType,Physics* physics)
{
	this->turrets.push_back(new Turret(x, z, weaponType,physics));
}

float TurretHandler::distanceToPlayer(const Vector3& position)
{
	float minDistance = -1;
	for (int i = 0; i < this->turrets.size(); i++)
	{
		float distance = (position - this->turrets[i]->getPosition()).Length();
		if (minDistance > distance || minDistance == -1)
		{
			minDistance = distance;
		}
	}
	return minDistance;
}

void TurretHandler::intersectPlayerBullets(Bullet* bulletArray, size_t size,float soundTimer)
{
	for (int i = 0; i < this->turrets.size(); i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (!this->turrets[i]->isDead())
			{
				if (bulletArray[j].getWeaponType() == WeaponType::Laser)
				{
					GameObject* laserObject = bulletArray[j].getGameObject();
					Vector3 rayDir = bulletArray[j].getDirection();
					Vector3 rayOrigin = laserObject->getPosition() - rayDir * laserObject->getScale().z;
					if (this->turrets[i]->getAABB().intersectXZ(rayOrigin, rayDir, laserObject->getScale().z, -1.0))
					{
						if (soundTimer > 0.05f) {
							Sound2::play("HitSound.wav");
							soundTimer = 0;
						}
						if (bulletArray[j].getFlame())// Damage over Time
						{
							turrets[i]->setFire(bulletArray[j].getFlameTimer());
						}
						if (bulletArray[j].getSplashBool())
						{
							for (int k = 0; k < turrets.size(); k++)
							{
								float deltaX = turrets[k]->getPosition().x - bulletArray[j].getGameObject()->getPosition().x;
								float deltaZ = turrets[k]->getPosition().z - bulletArray[j].getGameObject()->getPosition().z;
								float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
								if (k != i && distance < bulletArray[j].getSplashRange() && !turrets[k]->isDead())
								{
									turrets[k]->changeHealth(-bulletArray[j].getDamage() / (20 - Game::getGameInfo().nrOfClearedStages));
								}
							}
						}
						this->turrets[i]->changeHealth(-bulletArray[j].getDamage());
					}
				}
				else if (bulletArray[j].getMelee() && bulletArray[j].getGameObject()->getAABB().intersectXZ(this->turrets[i]->getAABB()))
				{
					if (soundTimer > 0.05f) {
						Sound2::play("HitSound.wav");
						soundTimer = 0;
					}
					if (bulletArray[j].getFlame())// Damage over Time
					{
						turrets[i]->setFire(bulletArray[j].getFlameTimer());
					}
					if (bulletArray[j].getSplashBool())
					{
						for (int k = 0; k < turrets.size(); k++)
						{
							float deltaX = turrets[k]->getPosition().x - bulletArray[j].getGameObject()->getPosition().x;
							float deltaZ = turrets[k]->getPosition().z - bulletArray[j].getGameObject()->getPosition().z;
							float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
							if (k != i && distance < bulletArray[j].getSplashRange() && !turrets[k]->isDead())
							{
								turrets[k]->changeHealth(-bulletArray[j].getDamage() / (20 - Game::getGameInfo().nrOfClearedStages));
							}
						}
					}
					this->turrets[i]->changeHealth(-bulletArray[j].getDamage());
				}
				else if (bulletArray[j].getGameObject()->getAABB().intersectXZ(this->turrets[i]->getAABB()))
				{
					if (soundTimer > 0.05f) {
						Sound2::play("HitSound.wav");
						soundTimer = 0;
					}
					if (bulletArray[j].getFlame())// Damage over Time
					{
						turrets[i]->setFire(bulletArray[j].getFlameTimer());
					}
					if (bulletArray[j].getSplashBool())
					{
						for (int k = 0; k < turrets.size(); k++)
						{
							float deltaX = turrets[k]->getPosition().x - bulletArray[j].getGameObject()->getPosition().x;
							float deltaZ = turrets[k]->getPosition().z - bulletArray[j].getGameObject()->getPosition().z;
							float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
							if (k != i && distance < bulletArray[j].getSplashRange() && !turrets[k]->isDead())
							{
								turrets[k]->changeHealth(-bulletArray[j].getDamage() / (20 - Game::getGameInfo().nrOfClearedStages));
							}
						}
					}
					this->turrets[i]->changeHealth(-bulletArray[j].getDamage());
					bulletArray[j].destroy();
				}
			}
		}
	}
}
