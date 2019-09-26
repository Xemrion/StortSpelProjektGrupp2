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