#pragma once
#include <queue>
#include "../GameObject.h"
#include <d3d11.h>
#include <SimpleMath.h>
#include "AStar.h"
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
	// nodes on the grid to walk on
	struct NodeList
	{
		Vector3 wayPoint;
	};
	// Set target
	void setTarget(Vector3 * target);
	// Move toward a target
	void findPath();
	// Move away from target
	void flee();
	// Move around randomly
	void wander();
	//Keep a certain distance from a target
	void separate();
	// Follow along a given path
	void followPath(float dt);
	// Set path List
	void setPath(std::queue<NodeList> Path);
	// Set path node
	void SetWayPoint(NodeList* Path);
	// Check if player is in line of sight
	bool lineOfSight(Vector3 &position);
	// Check if player is in range
	bool inRangeOf(Vector3 & position);
	AIPlayer();
	//bool isEmpty() { return this->Path.size(); }
	int getState();
private:
	bool Inside(Vector3 waypoint);
	AStar aStar;
	std::vector<Node*> nodePath;
	std::queue<NodeList*> Path;
	//use ref to  get currentTarget to use behaviour on
	Vector3 * currentTarget;
	Vector3 targetNode;
	int state;
};