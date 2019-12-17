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
	const AABB& scale(DirectX::SimpleMath::Vector3 scale)
	{
		AABB scaledAABB = { DirectX::SimpleMath::Vector3(minPos.x * scale.x, minPos.y * scale.y, minPos.z * scale.z),
							DirectX::SimpleMath::Vector3(maxPos.x * scale.x,maxPos.y * scale.y,maxPos.z * scale.z) };
		return scaledAABB;
	};



	bool intersect(const AABB& other)
	{
		if (minPos.x > other.maxPos.x || maxPos.x < other.minPos.x) return false;
		if (minPos.y > other.maxPos.y || maxPos.y < other.minPos.y) return false;
		if (minPos.z > other.maxPos.z || maxPos.z < other.minPos.z) return false;
		return true;
	}

	bool intersectXZ(const AABB& other)
	{
		if (minPos.x > other.maxPos.x || maxPos.x < other.minPos.x) return false;
		if (minPos.z > other.maxPos.z || maxPos.z < other.minPos.z) return false;
		return true;
	}

	bool intersect(DirectX::SimpleMath::Vector3 rayOrigin, DirectX::SimpleMath::Vector3 rayDir, float dist, float bias = 0.0)
	{
		DirectX::SimpleMath::Vector3 t0 = (minPos - rayOrigin) / rayDir;
		DirectX::SimpleMath::Vector3 t1 = (maxPos - rayOrigin) / rayDir;

		DirectX::SimpleMath::Vector3 minSlab = DirectX::SimpleMath::Vector3::Min(t0, t1);
		DirectX::SimpleMath::Vector3 maxSlab = DirectX::SimpleMath::Vector3::Max(t0, t1);

		float tmin = max(min(min(minSlab.x, maxSlab.x), min(minSlab.z, maxSlab.z)), min(minSlab.y, maxSlab.y));
		float tmax = max(max(max(minSlab.x, maxSlab.x), max(minSlab.z, maxSlab.z)), max(minSlab.y, maxSlab.y));;

		return tmin <= tmax && tmin <= dist && tmax >= bias;
	}

	bool intersectXZ(DirectX::SimpleMath::Vector3 rayOrigin, DirectX::SimpleMath::Vector3 rayDir, float dist, float bias = 0.0)
	{
		DirectX::SimpleMath::Vector3 t0 = (minPos - rayOrigin) / rayDir;
		DirectX::SimpleMath::Vector3 t1 = (maxPos - rayOrigin) / rayDir;

		DirectX::SimpleMath::Vector3 minSlab = DirectX::SimpleMath::Vector3::Min(t0, t1);
		DirectX::SimpleMath::Vector3 maxSlab = DirectX::SimpleMath::Vector3::Max(t0, t1);

		float tmin = max(min(minSlab.x, maxSlab.x), min(minSlab.z, maxSlab.z));
		float tmax = min(max(minSlab.x, maxSlab.x), max(minSlab.z, maxSlab.z));

		return tmin <= tmax && tmin <= dist && tmax >= bias;
	}
};
struct Sphere
{
	static bool intersection(DirectX::SimpleMath::Vector3& origin, float radius, DirectX::SimpleMath::Vector3 position)
	{
		DirectX::SimpleMath::Vector3 distance = origin - position;
		float fdist = (distance.x * distance.x + distance.y * distance.y + distance.z * distance.z);// distance.Dot(distance);
		if (fdist < (radius * radius))
		{
			return true;
		}
		return false;
	}
};
struct Material
{
	Texture* diffuse = nullptr;
	Texture* specular = nullptr;
	Texture* normal = nullptr;
	Texture* gloss = nullptr;
};

