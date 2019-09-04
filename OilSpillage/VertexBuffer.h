#ifndef VERTEXBUFFER_H
#define VERTEXBUFFER_H
#include<d3d11.h>
#include<wrl/client.h>
#include<memory>
template<class T>
class VertexBuffer
{
public:
	VertexBuffer()
	{

	}

	ID3D11Buffer* Get()const
	{
		return buffer.Get();
	}

	ID3D11Buffer* const* GetAddressOf()const
	{
		return buffer.GetAddressOf();
	}

	UINT getBufferSize()const
	{
		return this->bufferSize;
	}

	const UINT getStride()const
	{
		return *stride.get();
	}

	const UINT* getStridePtr()const
	{
		return stride.get();
	}

	HRESULT initialize(T* data, UINT numberOfVertices, ID3D11Device* device)
	{
		bufferSize = numberOfVertices;
		stride = std::make_unique<UINT>(sizeof(T));

		D3D11_BUFFER_DESC vBufferDesc;
		ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));

		vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBufferDesc.ByteWidth = sizeof(T) * numberOfVertices;
		vBufferDesc.CPUAccessFlags = 0;
		vBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		subData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vBufferDesc, &subData, buffer.GetAddressOf());

		return hr;
	}

	HRESULT initializeDynamic(T* data, UINT numberOfVertices, ID3D11Device* device)
	{
		this->data = data;
		this->dynamic = true;

		bufferSize = numberOfVertices;
		stride = std::make_unique<UINT>(sizeof(T));

		D3D11_BUFFER_DESC vBufferDesc;
		ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));

		vBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBufferDesc.ByteWidth = sizeof(T) * numberOfVertices;
		vBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vBufferDesc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subData;
		ZeroMemory(&subData, sizeof(subData));
		subData.pSysMem = data;

		HRESULT hr = device->CreateBuffer(&vBufferDesc, &subData, buffer.GetAddressOf());

		return hr;
	}

	bool applyChanges(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
	{
		if (dynamic)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = deviceContext->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			if (FAILED(hr))
			{
				return false;
			}

			CopyMemory(mappedResource.pData, data, sizeof(T) * bufferSize);
			deviceContext->Unmap(buffer.Get(), 0);
		}

		return true;
	}

private:
	VertexBuffer(const VertexBuffer<T>& rhs);

	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	std::unique_ptr<UINT> stride;
	UINT bufferSize = 0;

	//Dynamic stuff
	bool dynamic = false;
	T* data;
};
#endif // !VERTEXBUFFER_H

