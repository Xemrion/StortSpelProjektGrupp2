#include "..//game.h"
#include "Turrent.h"

Turret::Turret()
{
	this->sightRange = 10;
	this->setPosition(Vector3(-15.0f, 0.0f, 0.0f));
	setUpActor();
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
}

Turret::Turret(float x, float z, AStar* aStar) :Actor(x, z, aStar)
{
	this->setColor(Vector4(1.0f, 0.0f, 1.0f, 1.0f));
	this->sightRange = 10;
	//this->setPosition(Vector3(x, 0.0f, z));
	setUpActor();
	this->vecForward = Vector3(-1.0f, 0.0f, 0.0f);
}

void Turret::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;
	this->root->func();
	followPath();

	for (int i = 0; i < bulletCount; i++)
	{
		Game::getGraphics().removeFromDraw(this->bullets[i].obj);

		if (this->bullets[i].timeLeft > 0.0f)
		{
			this->bullets[i].timeLeft -= deltaTime;
			this->bullets[i].obj->move(this->bullets[i].dir * this->bullets[i].speed * deltaTime);
			Game::getGraphics().addToDraw(this->bullets[i].obj);
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
	inRange.addAction(std::bind(&Turrent::inRange, std::ref(*this)));

	Behavior& rotate = bt.getAction();
	rotate.addAction(std::bind(&Turrent::rotateToWards, std::ref(*this)));

	Behavior& shoot = bt.getAction();
	rotate.addAction(std::bind(&Turrent::shoot, std::ref(*this)));

	Behavior& idle = bt.getAction();
	idle.addAction(std::bind(&Turrent::idle, std::ref(*this)));

	//Attack shoot;

	root->addChildren(sequence);
	root->addChildren(idle);
	sequence.addChildren(inRange);
	sequence.addChildren(rotate);
	sequence.addChildren(shoot);
}

void Turret::followPath()
{
	destination = position;
}

Status Turret::rotateToWards()
{
	Status status = Status::SUCCESS;

	//float PI = 3.14159265358979323846;
	Vector3 dir = this->getPosition() - this->targetPos;
	//float pitch = 0.0f;
	//if (dir.y != 0.0f) {
	//	const float distance = sqrt(dir.x * dir.x + dir.z * dir.z);
	//	pitch = atan(dir.y / distance);
	//}
	//float yaw = 0.0f;
	//if (dir.x != 0.0f)
	//{
	//	yaw = atan(dir.x / dir.z);
	//}
	//if (dir.z > 0)
	//	yaw += PI;

	if ((dir - this->getPosition()).Length() > 0.01f)
	{
		vecForward = Vector3::Lerp(this->vecForward, dir, deltaTime);
		vecForward.Normalize();
	}
	else
	{
		this->vecForward = dir;
	}
	float newRot = atan2(this->vecForward.x, this->vecForward.z);
	this->setRotation(Vector3(0, newRot, 0));

	//Vector3 rotate(pitch, yaw, 0.f);
	//this->setRotation(rotate);

	return status;
}

Status Turret::inRange()
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

