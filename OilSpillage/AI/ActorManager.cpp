#include "ActorManager.h"

ActorManager::ActorManager()
{
}

ActorManager::~ActorManager()
{
}

void ActorManager::update(float dt)
{
	for (int i = 0; i < this->actors.size(); i++)
	{
		actors.at(i)->run(actors, dt);
	}
}
