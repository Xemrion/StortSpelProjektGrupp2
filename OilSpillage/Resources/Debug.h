#pragma once
#include "..//ConstantBuffer.h"
#include "..//VertexBuffer.h"
#include "..//IndexBuffer.h"
#include <DirectXMath.h>
#include<SimpleMath.h>
#include"..//Camera.h"
using namespace DirectX;

class Debug
{
private:
	struct CB_VS_World
	{
		DirectX::XMMATRIX wMatrix;
	};
	struct view
	{

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
	ID3D11Buffer* viewProjBuffer;
	Camera *camera;
public:
	Debug(ID3D11DeviceContext* dc, ID3D11Device* d, Camera camera);
	void DrawCube(DirectX::SimpleMath::Vector3 maxPos, DirectX::SimpleMath::Vector3 minPos,DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 color);
	void DrawLine(XMFLOAT3 p, XMFLOAT3 p2, XMFLOAT3 color);
	void DrawCube(XMFLOAT3 p, XMFLOAT3 color, float scale = 0.5f);
	void DrawRectangle(XMFLOAT3 center, XMFLOAT3 color, float scale = 0.5);
};
