#include "TestAI.h"

void TestAI::update(Vector3 playerPos, float dt, Vector3& unitPos, Vector4& color)
{
	if ((unitPos - playerPos).Length() < aggroRange && (unitPos - playerPos).Length() > attackRange)
	{
		unitPos -= (unitPos - playerPos) * dt;
	}
	else if ((unitPos - playerPos).Length() < attackRange)
	{
		color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	else
	{
		if(timer <= 0)
		{
			dir = (rand() % 4) + 1;
			timer = 1;
		}
		else
		{
			timer -= dt;
		}
		if (dir == 1)       //Left
		{
			unitPos -= (unitPos - (unitPos - Vector3(1.0f, 0.0f, 0.0f))) * dt;
		}
		else if (dir == 2) //Right
		{
			unitPos -= (unitPos - (unitPos + Vector3(1.0f, 0.0f, 0.0f))) * dt;
		}
		else if (dir == 3) //Up
		{
			unitPos -= (unitPos - (unitPos + Vector3(0.0f, 0.0f, 1.0f))) * dt;
		}
		else if (dir == 4) //Down
		{
			unitPos -= (unitPos - (unitPos - Vector3(0.0f, 0.0f, 1.0f))) * dt;
		}

	}
}
