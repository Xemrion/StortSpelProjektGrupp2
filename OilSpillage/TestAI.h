#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class TestAI
{
private:
	float attackRange = 1;
	float aggroRange = 5;
	bool attacked = false;
	float timer = 0;
	int dir = 1;

public:

	void update(Vector3 playerPos, float dt, Vector3& unitPos, Vector4& color);
};

