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
	//this->setColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	this->sightRange = 10;
	setUpActor();
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
	vecForward.Normalize();
	this->body.setPosition(this->position);
	this->body.setScale(this->scale);
	this->body.mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Turret");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Turret1");
	this->setTexture(Game::getGraphics().getTexturePointer("Entities/Dummy_Turret",true));
	this->body.setTexture(Game::getGraphics().getTexturePointer("Entities/Dummy_Turret", true));
	Game::getGraphics().addToDraw(&this->body);
	this->weapon.bulletSpeed = 30.0f;
	this->weapon.fireSpeed = 0.1f;
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
			if ((this->bullets[i].obj->getPosition() - this->targetPos).Length() < 0.5f)
			{
				static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->changeHealth(-5);
				this->bullets[i].timeLeft = 0;
				
			}
			else
			{
				Game::getGraphics().addParticle(this->position-this->vecForward, Vector3(0, 0, 0), 1, 1,1);
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
		vecForward -= (targetToSelf * deltaTime)/0.1;
		vecForward.Normalize();

		float newRot = atan2(this->vecForward.x, this->vecForward.z);
		this->setRotation(Vector3(0, newRot-(XM_PI/2), 0));
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

