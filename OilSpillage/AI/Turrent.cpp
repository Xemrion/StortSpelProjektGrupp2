#include "Turrent.h"

Turrent::Turrent()
{
	this->sightRange = 10;
	this->setPosition(Vector3(-15.0f, 0.0f, 0.0f));
	setUpActor();
	this->vecForward = Vector3(0.0f, 0.0f, 1.0f);
}

Turrent::Turrent(float x, float z, AStar* aStar) :Actor(x, z, aStar)
{
	this->setColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
	this->sightRange = 10;
	//this->setPosition(Vector3(x, 0.0f, z));
	setUpActor();
	this->vecForward = Vector3(0.0f, 0.0f, 1.0f);
}

void Turrent::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;
	this->root->func();
	followPath();
}

void Turrent::setForwardVector(Vector3 forward)
{
	this->vecForward = forward;
}

void Turrent::setSightRange(int range)
{
	this->sightRange = range;
}

void Turrent::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Turrent::inRange, std::ref(*this)));

	Behavior& rotate = bt.getAction();
	rotate.addAction(std::bind(&Turrent::rotateToWards, std::ref(*this)));

	Behavior& idle = bt.getAction();
	idle.addAction(std::bind(&Turrent::idle, std::ref(*this)));

	//Attack shoot;

	root->addChildren(sequence);
	root->addChildren(idle);
	sequence.addChildren(inRange);
	sequence.addChildren(rotate);
}

void Turrent::followPath()
{
	destination = position;
}

Status Turrent::rotateToWards()
{
	Status status = Status::SUCCESS;
	Vector3 Dir = this->targetPos - this->getPosition();
	Dir.Normalize();
	float value = Dir.Dot(vecForward);

	float angle = acosf(value);// vinkel

	float x = (cos(angle) * vecForward.x) + (sin(angle) * vecForward.z);
	float z = (cos(angle) * vecForward.z) - (sin(angle) * vecForward.x);
	vecForward.x = x;
	vecForward.z = z;
	this->setRotation(Vector3(0, 0, 1));

	return status;
}

Status Turrent::inRange()
{
	Status status = Status::INVALID;
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

Status Turrent::idle()
{
	Status status = Status::SUCCESS;
	return status;
}

