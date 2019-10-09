#include "ActorManager.h"

ActorManager::ActorManager()
{
}

ActorManager::~ActorManager()
{
	for(int i = 0; i < actors.size(); i++)
	{
		delete actors.at(i);
	}
	actors.clear();
}

void ActorManager::update(float dt, Vector3 targetPos)
{
	for (int i = 0; i < this->actors.size(); i++)
	{
		actors.at(i)->update(dt, targetPos);
		//actors.at(i)->run(actors, dt);
	}
}

void ActorManager::createDefender(float x, float z)
{
	this->actors.emplace_back(new Defender(x, z, this->aStar));
}

void ActorManager::createAttacker(float x, float z)
{
	this->actors.emplace_back(new Attacker(x, z, this->aStar));
}

void ActorManager::setAStar(AStar* aStar)
{
	this->aStar = aStar;
}
