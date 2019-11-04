#include "TurretHandler.h"
#include"..//Objectives/Objective.h"
#include"..//States/PlayingGameState.h"
TurretHandler::TurretHandler()
{
}
TurretHandler::~TurretHandler()
{
	for (int i = 0; i < turrets.size(); i++)
	{
		delete turrets[i];
	}
	turrets.clear();
}
void TurretHandler::update(float dt, DirectX::SimpleMath::Vector3 targetPos)
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
				turrets[i]->death();
				delete turrets[i];
				turrets.erase(turrets.begin() + i);
			}
		}
	}
}

void TurretHandler::createTurret(float x, float z)
{
	this->turrets.emplace_back(new Turret(x, z));
}

float TurretHandler::distanceToPlayer(DirectX::SimpleMath::Vector3 position)
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
				if (bulletArray[j].getGameObject()->getAABB().intersect(this->turrets[i]->getAABB()))
				{
					if (soundTimer > 0.05f) {
						/*int randomSound = rand() % 3 + 1;
						std::wstring soundEffect = L"data/sound/MetalImpactPitched" + to_wstring(randomSound) + L".wav";
						Sound::PlaySoundEffect(soundEffect);*/
						Sound::PlaySoundEffect(L"data/sound/HitSound.wav");
						soundTimer = 0;
					}
					this->turrets[i]->changeHealth(-bulletArray[j].getDamage());
					bulletArray[j].destroy();
				}
			}
		}
	}
}
