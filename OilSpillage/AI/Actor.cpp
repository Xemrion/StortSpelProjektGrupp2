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

void AIPlayer::setTarget(Vector3* target)
{
	this->currentTarget = target;
}

void AIPlayer::findPath()
{
	if (currentTarget != nullptr)
	{
		nodePath = aStar.algorithm(position, *currentTarget);
	}
}

void AIPlayer::followPath(float dt)
{
	if (nodePath.size() > 0)
	{
		targetNode = DirectX::SimpleMath::Vector3(nodePath.at(nodePath.size()-1)->GetXPos(), 0, nodePath.at(nodePath.size()-1)->GetYPos());
		Vector3 dir = targetNode - position;
		dir.Normalize();
		Vector3 newPosition = position + dir * dt;

		if (newPosition.Distance(targetNode, newPosition) < 1)
		{
			nodePath.pop_back();
		}

		this->setPosition(newPosition);
	}

	////check if we have a path to follow
	//if (Path.size() > 0)
	//{
	//	//check if we are at the waypoint
	//	Vector3 waypoint = this->Path.front()->wayPoint;
	//	if (Inside(waypoint))
	//	{
	//		lastPosition = this->GetPositionFloat3();
	//		// if we are inside lets pop waypoint
	//		this->Path.pop();
	//	}
	//	else
	//	{ // if we arent inside keep moving towards it
	//		
	//		// make a dir vector 
	//		Vector3 dir = waypoint - lastPosition;
	//		dir.Normalize();
	//		Vector3 position = this->GetPositionFloat3();
	//		Vector3 newPosition = position + dir * dt * 0.001;
	//		this->SetPosition(newPosition);
	//	}
	//}
	//else
	//{
	//	// if we dont have any nodes in the nodelist
	//	
	//	//call BT and ask for next behavoiour
	//}
}

void AIPlayer::SetWayPoint(NodeList* Path)
{
	this->Path.push(Path);
}

bool AIPlayer::inRangeOf(Vector3& position)
{
	return this->Inside(position);
}

AIPlayer::AIPlayer()
{
	aStar = AStar(10, 7);
	setPosition(DirectX::SimpleMath::Vector3(5, 0, 6));
	setTarget(&DirectX::SimpleMath::Vector3());
	findPath();
}

bool AIPlayer::Inside(Vector3 waypoint)
{
	//check if the actor is inside the waypoint
	bool inside = false;
	//Calculate the squared distance from the point to the center of the sphere
	Vector3 vecDist = waypoint - this->getPosition();
	float fDist = sqrtf(vecDist.Dot(vecDist));
	float Radius = 5;
	//Calculate if the squared distance between the sphere's center and the point
	//is less than the squared radius of the sphere
	if (fDist < Radius * Radius)
	{
		inside = true;
	}

	return inside;
}
