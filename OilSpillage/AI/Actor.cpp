#include "Actor.h"

void Actor::SetType(AItype type)
{
	this->type = type;
}

Actor::AItype& Actor::GetType()
{
	return this->type;
}

void AIPlayer::Update(float dt)
{
	//TODO: Call findPath every X frames
	//findPath();
	followPath(dt);
}

void AIPlayer::setPlayerPos(Vector3 playerPos)
{
	this->playerPos = playerPos;
}

void AIPlayer::findPath()
{
	//IF is chasing
	//path = aStar.algorithm(position, playerPos);
}

void AIPlayer::followPath(float dt)
{
	if (path.size() > 0)
	{
		targetNode = DirectX::SimpleMath::Vector3(path.at(path.size()-1)->GetXPos(), 0, path.at(path.size()-1)->GetYPos());
		Vector3 dir = targetNode - position;
		dir.Normalize();
		Vector3 newPosition = position + dir * dt;

		if (newPosition.Distance(targetNode, newPosition) < 1)
		{
			path.pop_back();
		}

		this->setPosition(newPosition);
	}
}
AIPlayer::AIPlayer()
{
	aStar = AStar(10, 7);
	setPosition(DirectX::SimpleMath::Vector3(5, 0, 6));
	setPlayerPos(&DirectX::SimpleMath::Vector3());
	findPath();
}
