#pragma once
#include<SimpleMath.h>
struct Vertex3D
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector2 uv;
	DirectX::SimpleMath::Vector3 normal;
};

struct CinematicPos
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 rotation;
	float time = 0.0f;
};

struct AABB
{
	DirectX::SimpleMath::Vector3 minPos;
	DirectX::SimpleMath::Vector3 maxPos;
	AABB scale(DirectX::SimpleMath::Vector3 scale)
	{
		AABB scaledAABB = { maxPos * scale, minPos * scale };
		return scaledAABB;
	};

	bool intersect(AABB other)
	{
		if (minPos.x > other.maxPos.x || maxPos.x < other.minPos.x) return false;
		if (minPos.y > other.maxPos.y || maxPos.y < other.minPos.y) return false;
		if (minPos.z > other.maxPos.z || maxPos.z < other.minPos.z) return false;
		return true;
	}
};
