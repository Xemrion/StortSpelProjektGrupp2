#include "fog.h"
#include "States/PlayingGameState.h"

using namespace DirectX::SimpleMath;

Fog::~Fog()
{
	while (quads.size() > 0)
	{
		delete quads[quads.size() - 1];
		quads.erase(quads.end() - 1);
	}

	while (fogTextures.size() > 0)
	{
		delete fogTextures[fogTextures.size() - 1];
		fogTextures.erase(fogTextures.end() - 1);
	}

	while (normalTextures.size() > 0)
	{
		delete normalTextures[normalTextures.size() - 1];
		normalTextures.erase(normalTextures.end() - 1);
	}
}

void Fog::initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slices, float spacing)
{
	Game::getGraphics().loadMesh("Quad");
	Game::getGraphics().loadShape(SHAPE_QUAD);
	Game::getGraphics().loadTexture("perlin_s4_e8");
	noiseTexture = Game::getGraphics().getTexturePointer("perlin_s4_e8");

	//Game::getGraphics().loadTexture("brickwall");
	//noiseTexture = Game::getGraphics().getTexturePointer("brickwall");

	for (UINT i = 1; i <= slices; ++i)
	{
		GameObject* q = new GameObject();
		q->mesh = Game::getGraphics().getMeshPointer("Quad");
		q->setPosition(Vector3(95*10, i * spacing - 1.5f, -95*10));
		//spacing *= 0.95;
		q->setScale(Vector3(96.0 * 10, 96.0 * 10, 1.0));
		q->setColor(Vector4(0.0, 0.0, 0.0, (float(i) / slices)));
		//Game::getGraphics().loadTexture("brickwallnormal");
		//Game::getGraphics().loadTexture("brickwall");
		Game::getGraphics().loadTexture("white1x1");
		q->setGlossMap(Game::getGraphics().getTexturePointer("white1x1"));
		q->setSpecularMap(Game::getGraphics().getTexturePointer("white1x1"));
		quads.push_back(q);
	}

	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
#ifdef _DEBUG //Debug Mode
#ifdef _WIN64 //x64
	shaderfolder = L"..\\x64\\Debug\\";
#else  //x86 (Win32)
	shaderfolder = L"..\\Debug\\";
#endif
#else //Release Mode
#ifdef _WIN64 //x64
	shaderfolder = L"..\\x64\\Release\\";
#else  //x86 (Win32)
	shaderfolder = L"..\\Release\\";
#endif
#endif

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"SV_POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  }
	};
	UINT numElements = ARRAYSIZE(inputDesc);

	this->shader.createVS(device.Get(), shaderfolder + L"SimpleVS.cso", inputDesc, numElements);
	this->shader.createPS(device.Get(), shaderfolder + L"FogPS.cso");

	this->vp.Width = noiseTexture->getWidth();
	this->vp.Height = noiseTexture->getHeight();
	this->vp.MinDepth = 0.0f;
	this->vp.MaxDepth = 1.0f;
	this->vp.TopLeftX = 0;
	this->vp.TopLeftY = 0;
	
	generateTextures(device, deviceContext);
}

void Fog::generateTextures(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext)
{
	float clearColor[4] = {
		0,0,0,0
	};

	deviceContext->RSSetViewports(1, &this->vp);

	deviceContext->PSSetShader(this->shader.ps.getShader(), nullptr, 0);
	deviceContext->VSSetShader(this->shader.vs.getShader(), nullptr, 0);

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->IASetInputLayout(this->shader.vs.getInputLayout());

	Microsoft::WRL::ComPtr<ID3D11Buffer> viewProjBuffer;
	D3D11_BUFFER_DESC desc = { 0 };
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(Matrix) + (16 - (sizeof(Matrix) % 16)));
	desc.StructureByteStride = 0;
	device->CreateBuffer(&desc, 0, &viewProjBuffer);
	deviceContext->VSSetConstantBuffers(0, 1, viewProjBuffer.GetAddressOf());


	Microsoft::WRL::ComPtr<ID3D11Buffer> worldBuffer;
	desc.ByteWidth = static_cast<UINT>(sizeof(Matrix) + (16 - (sizeof(Matrix) % 16)));
	device->CreateBuffer(&desc, 0, &worldBuffer);
	deviceContext->VSSetConstantBuffers(1, 1, worldBuffer.GetAddressOf());

	struct FogMaterial
	{
		Vector3 color;
		float densityThreshold;
		float density;
		float ambientDensity;
		float normalY;
		float padding;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer> materialBuffer;
	desc.ByteWidth = static_cast<UINT>(sizeof(FogMaterial));
	device->CreateBuffer(&desc, 0, &materialBuffer);
	deviceContext->VSSetConstantBuffers(0, 1, materialBuffer.GetAddressOf());

	Microsoft::WRL::ComPtr <ID3D11SamplerState> sampler;
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;



	SimpleMath::Matrix viewProj = Matrix(
		Vector4(1.0, 0.0, 0.0, 0.0),
		Vector4(0.0, 1.0, 0.0, 0.0),
		Vector4(0.0, 0.0, 0.0, 1.0),
		Vector4(0.0, 0.0, 0.0, 0.0));
	SimpleMath::Matrix viewProjTr = DirectX::XMMatrixTranspose(viewProj);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(viewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &viewProjTr, sizeof(SimpleMath::Matrix));
	deviceContext->Unmap(viewProjBuffer.Get(), 0);



	device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());
	fogTextures.clear();
	for (UINT i = 0; i < quads.size(); ++i)
	{
		GameObject* object = quads[i];
		UINT vertexCount = object->mesh->getVertexCount();
		UINT stride = sizeof(Vertex3D);
		UINT offset = 0;
		deviceContext->IASetVertexBuffers(0, 1, object->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		
		SimpleMath::Matrix world = Matrix::CreateTranslation(Vector3(0.0, 0.0, 1.0));
		SimpleMath::Matrix worldTr = DirectX::XMMatrixTranspose(world);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &worldTr, sizeof(SimpleMath::Matrix));
		deviceContext->Unmap(worldBuffer.Get(), 0);

		FogMaterial material;
		Vector4 color = object->getColor();
		material.color = Vector3(color);
		//material.densityThreshold = color.w;
		material.densityThreshold = 0.075f;
		material.ambientDensity = 0.015f;
		material.density = 0.15f;
		
		auto smoothstep = [](float edge0, float edge1, float x)
		{
			float t = clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
			return t * t * (3.0f - 2.0f * t);
		};

		material.normalY = smoothstep(quads[quads.size() - 1]->getPosition().y, quads[0]->getPosition().y, object->getPosition().y) - 0.5;
		material.normalY = object->getPosition().y;
		hr = deviceContext->Map(materialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &material, sizeof(FogMaterial));
		deviceContext->Unmap(materialBuffer.Get(), 0);
		object->setColor(Vector4(0.6, 0.6, 0.62, 0.0));


		Texture* diffuseTexture = new Texture();
		diffuseTexture->Initialize(device.Get(), deviceContext.Get(), "data/textures/perlin_s4_e8.tga", 1, false);
		Texture* normalMap = new Texture();
		normalMap->Initialize(device.Get(), deviceContext.Get(), "data/textures/perlin_s4_e8.tga", 1, false);
		device.Get()->CreateRenderTargetView(diffuseTexture->getTexture2D(), NULL, textureRTV.ReleaseAndGetAddressOf());
		device.Get()->CreateRenderTargetView(normalMap->getTexture2D(), NULL, normalRTV.ReleaseAndGetAddressOf());
		deviceContext.Get()->ClearRenderTargetView(textureRTV.Get(), clearColor);
		deviceContext.Get()->ClearRenderTargetView(normalRTV.Get(), clearColor);
		ID3D11RenderTargetView* RTVarray[2]{ textureRTV.Get(), normalRTV.Get() };
		deviceContext.Get()->OMSetRenderTargets(2, RTVarray, NULL);



		auto srv = noiseTexture->getShaderResView();
		deviceContext->PSSetShaderResources(0, 1, &srv);
		deviceContext->PSSetSamplers(0, 1, sampler.GetAddressOf());
		deviceContext->VSSetConstantBuffers(0, 1, viewProjBuffer.GetAddressOf());
		deviceContext->VSSetConstantBuffers(1, 1, worldBuffer.GetAddressOf());
		deviceContext->PSSetConstantBuffers(0, 1, materialBuffer.GetAddressOf());
		deviceContext->Draw(vertexCount, 0);

		fogTextures.push_back(diffuseTexture);
		normalTextures.push_back(normalMap);
		object->setRotation(Vector3(XM_PIDIV2, 0.0, 0.0));
	}

	for (int i = 0; i < quads.size(); ++i)
	{
		quads[i]->setTexture(fogTextures[i]);
		//quads[i]->setNormalMap(normalTextures[i]);
	}
}


std::vector<GameObject*>& Fog::getQuads()
{
	return quads;
}
