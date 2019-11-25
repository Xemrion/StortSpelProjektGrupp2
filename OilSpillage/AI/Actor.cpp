#include "../States/PlayingGameState.h"
#include "Actor.h"

Actor::Actor()
{
	this->setUpActor();

	this->velocity = Vector3(10.0f, 0.0f, 10.0f);
	this->position = Vector3(0, 0.0f, 0);
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
	this->points = 0;
}

Actor::Actor(float x, float z,Physics* physics)
{
	this->velocity = Vector3(10.0f, 0.0f, 10.0f);
	this->position = Vector3(x, -1.0f, z);
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
	this->points = 0;
}
Actor::~Actor()
{
	for(int i = 0; i < 60; i++)
	{
		Game::getGraphics().addParticle(position, Vector3(0.0f), 25, 3);
	}
}

void Actor::update(float dt, const Vector3& targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;
	this->root->func();
	if(isHit)
	{
		setColor(Vector4(getColor().x / (1 + 15.0f * deltaTime), getColor().y, getColor().z, 1));
		if (getColor().x <= 0.01f) 
		{
			isHit = false;
		}
	}
}
//
//void Actor::run(vector<Actor*>& boids, float deltaTime, vector<Vector3> buildings, Vector3 targetPos)
//{
//	applyForce(separation(boids, buildings, targetPos) * 4);
//	update(deltaTime, targetPos);
//	if (this->state != State::Idle)
//	{
//		move();
//	}
//}

void Actor::setHealth(float health)
{
	this->health = std::clamp(health, 0.0f, this->stats.maxHealth);
}

void Actor::changeHealth(float amount)
{
	if (amount < 0) {
		isHit = true;
	}
	setColor(Vector4(max(getColor().x + -amount * 0.1f, 0), getColor().y, getColor().z, 1));
	this->health = std::clamp(this->health + amount, 0.0f, this->stats.maxHealth);
	Game::getGraphics().addParticle2(this->getPosition(), Vector3(0, 0, 0), 2, 1);
}

bool Actor::isDead() const
{
	return this->health <= 0;
}

int Actor::getPoints()
{
	return points;
}

void Actor::setPoints(int amount)
{
	this->points = amount;
}
