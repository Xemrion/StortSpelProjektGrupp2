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

	vertexBuffer.initialize(vertices.data(), vertices.size(), device);
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
