#pragma once
#include "Defender.h"
#include "Attacker.h"
class ActorManager
{
public:
	ActorManager();
	~ActorManager();
	void update(float dt);
private:
	std::vector<Actor*> actors;
};