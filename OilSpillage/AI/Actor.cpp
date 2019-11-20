#include "../States/PlayingGameState.h"
#include "Actor.h"

Actor::Actor()
{
	this->setUpActor();

	this->velocity = Vector3(10.0f, 0.0f, 10.0f);
	this->position = Vector3(0, 0.0f, 0);
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
}

Actor::Actor(float x, float z,Physics* physics)
{
	this->velocity = Vector3(10.0f, 0.0f, 10.0f);
	this->position = Vector3(x, -1.0f, z);
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
}
Actor::~Actor()
{
}

void Actor::update(float dt, const Vector3& targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;
	this->root->func();
}

void Actor::setHealth(int health)
{
	this->health = std::clamp(health, 0, this->stats.maxHealth);
}

void Actor::changeHealth(int amount)
{
	this->health = std::clamp(this->health + amount, 0, this->stats.maxHealth);
	Game::getGraphics().addParticle2(this->getPosition(), Vector3(0, 0, 0), 2, 1);
}

bool Actor::isDead() const
{
	return this->health <= 0;
}



