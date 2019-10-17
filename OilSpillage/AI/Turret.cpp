#include "../States/PlayingGameState.h"
#include "Turret.h"

Turret::Turret()
{
	this->sightRange = 10;
	this->setPosition(Vector3(-15.0f, 0.0f, 0.0f));
	setUpActor();
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
}

Turret::Turret(float x, float z) 
	: Actor(x, z, nullptr)
{
	this->setColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
	this->sightRange = 10;
	setUpActor();
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
	vecForward.Normalize();
}

void Turret::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;
	this->root->func();

	rotateTowards();

	for (int i = 0; i < bulletCount; i++)
	{
		Game::getGraphics().removeFromDraw(this->bullets[i].obj);

		if (this->bullets[i].timeLeft > 0.0f)
		{
			if ((this->position - this->targetPos).Length() < 5.0f)
			{
				static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->changeHealth(-20);
				this->bullets[i].timeLeft = 0;
			}
			else
			{
				this->bullets[i].timeLeft -= deltaTime;
				this->bullets[i].obj->move(this->bullets[i].dir * this->bullets[i].speed * deltaTime);
				Game::getGraphics().addToDraw(this->bullets[i].obj);
			}
		}
	}
}

void Turret::setForwardVector(Vector3 forward)
{
	this->vecForward = forward;
}

void Turret::setSightRange(int range)
{
	this->sightRange = range;
}

void Turret::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Turret::inRange, std::ref(*this)));

	Behavior& rotate = bt.getAction();
	rotate.addAction(std::bind(&Turret::rotateTowards, std::ref(*this)));

	Behavior& shoot = bt.getAction();
	rotate.addAction(std::bind(&Turret::shoot, std::ref(*this)));

	Behavior& idle = bt.getAction();
	idle.addAction(std::bind(&Turret::idle, std::ref(*this)));

	//Attack shoot;

	root->addChildren(sequence);
	root->addChildren(idle);
	sequence.addChildren(inRange);
	sequence.addChildren(rotate);
	sequence.addChildren(shoot);
}

void Turret::followPath()
{
}
void Turret::updateBoid(float deltatime)
{
}

Status Turret::rotateTowards()
{
	Vector3 targetToSelf = (targetPos - position);
	targetToSelf.Normalize();

	if ((targetToSelf).Dot(vecForward) < 0.8)
	{
		vecForward -= (targetToSelf * deltaTime)/1.5;
		vecForward.Normalize();

		float newRot = atan2(this->vecForward.x, this->vecForward.z);
		this->setRotation(Vector3(0, newRot, 0));
	}
	return Status::SUCCESS;
}

Status Turret::inRange()
{
	Status status;
	if ((getPosition() - targetPos).Length() > this->sightRange)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

Status Turret::idle()
{
	return Status::SUCCESS;
}

