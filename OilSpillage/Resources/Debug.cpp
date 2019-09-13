#include "Debug.h"

Debug::Debug(ID3D11DeviceContext* dc, ID3D11Device *d)
{
	//initilizelie the buffer
	this->deviceContext = dc;
	this->device = d;
	this->cb_vs_world.initialize(device);
}

void Debug::DrawLine(XMFLOAT3 start, XMFLOAT3 end, XMFLOAT3 color)
{
	std::vector<DebugVertex> vertices;
	VertexBuffer<DebugVertex>vertexBuffer;
	vertices.resize(2);
	{
		vertices[0] = { start ,	color };
		vertices[1] = { end , color };
	}

	vertexBuffer.initialize(vertices.data(), static_cast<UINT>(vertices.size()), device);
	vertexBuffer.applyChanges(device, deviceContext);

	//set world buffer
	XMMATRIX world = XMMatrixIdentity();
	cb_vs_world.data.wMatrix = world;
	cb_vs_world.applyChanges(device, deviceContext);
	deviceContext->VSSetConstantBuffers(1, 1, this->cb_vs_world.getAddressOfBuffer());
	//draw
	UINT stride = sizeof(DebugVertex);
	UINT offset = 0;
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.getStridePtr(), &offset);
	deviceContext->Draw(2, 0);

}

void Debug::DrawCube(XMFLOAT3 p, XMFLOAT3 color, float scale)
{
	std::vector<DebugVertex> vertices;
	VertexBuffer<DebugVertex>vertexBuffer;
	IndexBuffer indicesbuffer;
	vertices.resize(8);
	{
		vertices[0] = { XMFLOAT3(-scale + p.x ,  scale + p.y, -scale + p.z), color };
		vertices[1] = { XMFLOAT3(-scale + p.x, -scale + p.y, -scale + p.z),	color };
		vertices[2] = { XMFLOAT3(scale + p.x, -scale + p.y, -scale + p.z),	color };
		vertices[3] = { XMFLOAT3(scale + p.x,  scale + p.y, -scale + p.z),	color };
		vertices[4] = { XMFLOAT3(-scale + p.x, -scale + p.y,  scale + p.z),	color };
		vertices[5] = { XMFLOAT3(-scale + p.x,  scale + p.y,  scale + p.z),	color };
		vertices[6] = { XMFLOAT3(scale + p.x, -scale + p.y,  scale + p.z),  color };
		vertices[7] = { XMFLOAT3(scale + p.x,  scale + p.y,  scale + p.z),	color };
	}
	std::vector<DWORD> indexData =
	{
		0, 1, 2, 2, 3, 0,
		4, 1, 0, 0, 5, 4,
		2, 6, 7, 7, 3, 2,
		4, 5, 7, 7, 6, 4,
		0, 3, 7, 7, 5, 0,
		1, 4, 2, 2, 4, 6
	};
	vertexBuffer.initialize(vertices.data(), static_cast<UINT>(vertices.size()), device);
	vertexBuffer.applyChanges(device, deviceContext);

	indicesbuffer.Initialize(device, indexData.data(), static_cast<UINT>(indexData.size()));
	//set world buffer
	XMMATRIX world = XMMatrixIdentity();
	cb_vs_world.data.wMatrix = world;
	cb_vs_world.applyChanges(device, deviceContext);
	deviceContext->VSSetConstantBuffers(1, 1, this->cb_vs_world.getAddressOfBuffer());
	//draw
	UINT stride = sizeof(DebugVertex);
	UINT offset = 0;
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.getStridePtr(), &offset);
	/*deviceContext->Draw(2, 0);*/
	deviceContext->IASetIndexBuffer(indicesbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(indicesbuffer.IndexCount(), 0, 0);
}

void Debug::DrawRectangle(XMFLOAT3 center, XMFLOAT3 color, float scale)
{std::vector<DebugVertex> vertices;
	VertexBuffer<DebugVertex>vertexBuffer;
	IndexBuffer indicesbuffer;
	vertices.resize(4);
	{
		vertices[0] = { XMFLOAT3(-scale + center.x , center.y, -scale + center.z), color };
		vertices[1] = { XMFLOAT3(-scale + center.x,center.y, -scale + center.z),	color };
		vertices[2] = { XMFLOAT3(scale + center.x, center.y, scale + center.z),	color };
		vertices[3] = { XMFLOAT3(scale + center.x,  center.y, scale + center.z),	color };

	}
	std::vector<DWORD> indexData =
	{
		0, 1,
		1, 2,
		2, 3,
		3, 0 
	};
	vertexBuffer.initialize(vertices.data(), static_cast<UINT>(vertices.size()), device);
	vertexBuffer.applyChanges(device, deviceContext);
	indicesbuffer.Initialize(device, indexData.data(), static_cast<UINT>(indexData.size()));
	//set world buffer
	XMMATRIX world = XMMatrixIdentity();
	cb_vs_world.data.wMatrix = world;
	cb_vs_world.applyChanges(device, deviceContext);
	deviceContext->VSSetConstantBuffers(1, 1, this->cb_vs_world.getAddressOfBuffer());
	//draw
	UINT stride = sizeof(DebugVertex);
	UINT offset = 0;
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), vertexBuffer.getStridePtr(), &offset);
	deviceContext->IASetIndexBuffer(indicesbuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(indicesbuffer.IndexCount(), 0, 0);
}
