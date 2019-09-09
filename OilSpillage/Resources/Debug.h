#pragma once
#include "..//ConstantBuffer.h"
#include "..//VertexBuffer.h"
#include "..//IndexBuffer.h"
#include <DirectXMath.h>
using namespace DirectX;

class Debug
{
private:
	struct CB_VS_World
	{
		DirectX::XMMATRIX wMatrix;
	};
	struct DebugVertex
	{
		DebugVertex() { };
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
	void DrawLine(XMFLOAT3 p, XMFLOAT3 p2, XMFLOAT3 color);
	void DrawCube(XMFLOAT3 p, XMFLOAT3 color, float scale = 0.5f);
	void DrawRectangle(XMFLOAT3 center, XMFLOAT3 color, float scale = 0.5);
};
