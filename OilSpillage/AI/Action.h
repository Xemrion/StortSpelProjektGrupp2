#ifndef ACTION_H
#define ACTION_H

#include <queue>
#include <d3d11.h>
#include <SimpleMath.h>

#include "Behaviour.h"
using namespace DirectX::SimpleMath;

enum AIState
{
	chasing,
	attacking,
	wandering
};

class Action
{
public:
	Action(class Actor* owner);
	~Action();

	Status chase();
	Status roam();
	Status attack();
	Status inRange();
	Status inAggroRange();

	void update(float dt, Vector3 targetPos);
	AIState getState();

private:
	class Actor* owner;
	float deltaTime;
	Vector3 targetPos;
	AIState state;

};
#endif // !