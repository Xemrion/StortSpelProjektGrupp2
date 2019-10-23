#include "../States/PlayingGameState.h"
#include "Turret.h"

Turret::Turret()
{
	this->sightRange = 10;
	this->setPosition(Vector3(-15.0f, 0.0f, 0.0f));
	setUpActor();
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
	this->weapon = WeaponHandler::getWeapon(WeaponType::aiMachineGun);
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
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Dummy_Turret"));
	this->body.setMaterial(Game::getGraphics().getMaterial("Entities/Dummy_Turret"));
	Game::getGraphics().addToDraw(&this->body);

	this->weapon = WeaponHandler::getWeapon(WeaponType::aiMachineGun);
}

void Turret::update(float dt, Vector3 targetPos)
{
	this->velocity = Vector3(0.0, 0.0, 0.0);
	this->deltaTime = dt;
	this->targetPos = targetPos;
	this->root->func();

	rotateTowards();

	updateWeapon(deltaTime);
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

