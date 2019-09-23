#pragma once
#include <queue>
#include "../GameObject.h"
#include <d3d11.h>
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

// https://docs.unrealengine.com/en-US/API/Runtime/AIModule/AAIController/index.html
class Actor
{
public:
	enum AItype //testa typer
	{
		Wanderer,
		chaser,
	};
	void SetType(AItype type);
	AItype& GetType();
private:
	AItype type;
};


class AIPlayer :public GameObject
{
public:
	void Update(float dt);
public:
	// nodes on the grid to walk on
	struct NodeList
	{
		Vector3 wayPoint;
	};
	// Set target
	void SetTarget(Vector3 * target);
	// Move toward a target
	void Seek(float dt);
	// Move away from target
	void Flee();
	// Move around randomly
	void Wander();
	//Keep a certain distance from a target
	void Separate();
	// Follow along a given path
	void PathFollowing(float dt);
	// Set path List
	void SetPath(std::queue<NodeList> Path);
	// Set path node
	void SetWayPoint(NodeList* Path);
	// Check if player is in line of sight
	bool LineOfSight(Vector3 &position);
	// Check if player is in range
	bool InRangeOf(Vector3 & position);

	//bool isEmpty() { return this->Path.size(); }
	int GetState();
private:
	bool Inside(Vector3 waypoint);

	std::queue<NodeList*> Path;
	//use ref to  get currentTarget to use behaviour on
	Vector3 * currentTarget;
	int state;
};