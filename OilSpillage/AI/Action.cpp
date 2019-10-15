
#include "Action.h"
#include "..//game.h"
#include "Actor.h"
Action::Action(Actor* owner):owner(owner)
{
	owner->setPosition(DirectX::SimpleMath::Vector3(5, 0, 6));
	//setTargetPos(DirectX::SimpleMath::Vector3());
	state = AIState::chasing;
	this->deltaTime = 0;
}

Action::~Action()
{
	this->owner = nullptr;
	deltaTime = 0;
}

void Action::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;
}

AIState Action::getState()
{
	return state;
}

Status Action::roam()
{
	Status status;
	if (owner->getPathSize() > 0)
	{
		owner->roam();
		status = Status::SUCCESS;

	}
	else
	{
		owner->findPath(targetPos);
		status = Status::FAILURE;
	}
	return status;
}

Status Action::attack()
{
	owner->shoot(deltaTime, targetPos);
	return Status::SUCCESS;
}

Status Action::chase()
{
	owner->chase();
	return Status::SUCCESS;
}

Status Action::inRange()
{
	Status status;

	if ((owner->getPosition() - targetPos).Length() > 10)
	{
		status = Status::FAILURE;
	}

	else
	{
		status = Status::SUCCESS;
	}
	return status;
}

Status Action::inAggroRange()
{
	Status status;

	if ((owner->getPosition() - targetPos).Length() > 100)
	{
		return status = Status::FAILURE;
	}

	else
	{
		status = Status::SUCCESS;
		return status;
	}

}
