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

	enum AIState
	{
		chasing,
		attacking,
		wandering
	};
	void Update(float dt);
	void setPlayerPos(Vector3 playerPos);
	// Move toward a target
	void findPath();
	// Move away from target
	void flee();
	// Move around randomly
	void wander();
	//Keep a certain distance from a target
	void separate();
	void followPath(float dt);
	bool lineOfSight(Vector3& position);
	AIPlayer();
	~AIPlayer();
	int getState();
private:
	AStar* aStar;
	std::vector<Node*> path;
	//use ref to  get currentTarget to use behaviour on
	Vector3 playerPos;
	Vector3 targetNode;
	AIState state;
};