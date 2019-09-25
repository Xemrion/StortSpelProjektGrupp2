#pragma once
#include "..//ConstantBuffer.h"
#include "..//VertexBuffer.h"
#include "..//IndexBuffer.h"
#include <DirectXMath.h>
#include<SimpleMath.h>
#include"..//Camera.h"
#include "../GameObject.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

class Debug
{
private:
	struct CB_VS_World
	{
		DirectX::XMMATRIX wMatrix;
	};
	struct DebugVertex
	{
		DebugVertex() : pos(0.0f, 0.0f, 0.0f), color(0.0f, 0.0f, 0.0f) { };
		DebugVertex(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3  color)
			: pos(position), color(color) {}

		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};
	ConstantBuffer<CB_VS_World> cb_vs_world;

	ID3D11DeviceContext* deviceContext;
	ID3D11Device* device;
public:
	Debug(ID3D11DeviceContext* dc, ID3D11Device* d);
	void DrawAABB(GameObject* obj, Vector3 color);
	void DrawCube(Vector3 maxPos, Vector3 minPos, Vector3 pos, Vector3 color);
	void DrawLine(Vector3 p, Vector3 p2, Vector3 color);
	void DrawCube(Vector3 p, Vector3 color, float scale = 0.5f);
	void DrawRectangle(Vector3 center, Vector3 color, float scale = 0.5);
};
