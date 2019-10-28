#pragma once
#include<SimpleMath.h>
#include "texture.h"

struct Vertex3D
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector2 uv;
	DirectX::SimpleMath::Vector3 normal;
	DirectX::SimpleMath::Vector3 tangent;
	DirectX::SimpleMath::Vector3 bitangent;
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
		AABB scaledAABB = { minPos * scale, maxPos * scale };
		return scaledAABB;
	};

	

	bool intersect(AABB other)
	{
		if (minPos.x > other.maxPos.x || maxPos.x < other.minPos.x) return false;
		if (minPos.y > other.maxPos.y || maxPos.y < other.minPos.y) return false;
		if (minPos.z > other.maxPos.z || maxPos.z < other.minPos.z) return false;
		return true;
	}

	bool intersectXZ(AABB other)
	{
		if (minPos.x > other.maxPos.x || maxPos.x < other.minPos.x) return false;
		if (minPos.z > other.maxPos.z || maxPos.z < other.minPos.z) return false;
		return true;
	}
};

struct Material
{
	Texture* diffuse = nullptr;
	Texture* specular = nullptr;
	Texture* normal = nullptr;
	Texture* gloss = nullptr;
};

