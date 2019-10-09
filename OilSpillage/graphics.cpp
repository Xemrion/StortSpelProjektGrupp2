#include "graphics.h"
#include <algorithm>
#include"Importer/importerClass.h"
#include<cstring>
#include "ShaderDefines.hlsli"
#include "UI/UserInterface.h"
#include <cassert>

Graphics::Graphics()
{
	this->vp = {};

	this->debugger = nullptr;

	this->lightList = nullptr;

	lightBufferContents = new LightBufferContents;
}

Graphics::~Graphics()
{
	if(debug!=nullptr)
		debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);

	delete this->debugger;

	for (auto i = textures.begin(); i != textures.end(); i++)
	{
		delete i->second;
	}

	delete this->lightBufferContents;
}

bool Graphics::init(Window* window)
{
	this->window = window;
	HRESULT result;

	D3D11_BLEND_DESC blendStateDescription;
	
	DXGI_SWAP_CHAIN_DESC swapchainDesc;
	ZeroMemory(&swapchainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapchainDesc.BufferDesc.Width = this->window->width;
	swapchainDesc.BufferDesc.Height = this->window->height;
	swapchainDesc.BufferCount = 1;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.OutputWindow = this->window->handle;
	swapchainDesc.SampleDesc.Count = 4;
	swapchainDesc.Windowed = true;

	D3D11_CREATE_DEVICE_FLAG deviceFlags = (D3D11_CREATE_DEVICE_FLAG)0;
#if _DEBUG
	deviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	result = D3D11CreateDeviceAndSwapChain(NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		deviceFlags,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&swapchainDesc,
		swapChain.ReleaseAndGetAddressOf(),
		device.ReleaseAndGetAddressOf(),
		NULL,
		deviceContext.ReleaseAndGetAddressOf());


		// get the address of the back buffer
		ID3D11Texture2D* backBufferPtr = nullptr;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& backBufferPtr);
		if (FAILED(result) || backBufferPtr == nullptr)
		{
			MessageBox(this->window->handle, "Could not ID3D11Texture2D* backBufferPtr", "Error", MB_OK); //L"", L"", ;
			return false;
		}

		// use the back buffer address to create the render target
		device->CreateRenderTargetView(backBufferPtr, NULL, renderTargetView.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			MessageBox(this->window->handle, "Could not ID3D11Texture2D* backBufferPtr", "Error", MB_OK);
			return false;
		}
		backBufferPtr->Release();
		backBufferPtr = nullptr;

		//swapChain->SetFullscreenState(true, nullptr);//testing fullscreen

		D3D11_TEXTURE2D_DESC descDepth;
		descDepth.Width = (UINT)this->window->width;
		descDepth.Height = (UINT)this->window->height;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 4;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;


		
		result = device->CreateTexture2D(&descDepth, NULL, &depthStencilBuffer);
		if (FAILED(result))
		{
			return false;
		}
		result = device->CreateDepthStencilView(depthStencilBuffer.Get(), NULL, &depthStencilView);
		if (FAILED(result))
		{
			return false;
		}
		deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());

		//the depth Stencil State
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		// Depth test parameters
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		// Create depth stencil state
		result = device->CreateDepthStencilState(&depthStencilDesc, depthStencilState.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			return false;
		}

	

	this->vp.Width = (float)this->window->width;
	this->vp.Height = (float)this->window->height;
	this->vp.MinDepth = 0.0f;
	this->vp.MaxDepth = 1.0f;
	this->vp.TopLeftX = 0;
	this->vp.TopLeftY = 0;
	deviceContext->RSSetViewports(1, &this->vp);


	D3D11_BUFFER_DESC desc = { 0 };

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(Matrix) + (16 - (sizeof(Matrix) % 16)));
	desc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&desc, 0, &viewProjBuffer);
	if (FAILED(hr))
		return false;

	hr = device->CreateBuffer(&desc, 0, &worldBuffer);
	if (FAILED(hr))
		return false;


	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(Vector4) + (16 - (sizeof(Vector4) % 16)));
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, &colorBuffer);
	if (FAILED(hr))
		return false;

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(Vector4) * 2);
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, &sunBuffer);
	if (FAILED(hr))
		return false;

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(LightBufferContents));
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, &lightBuffer);
	if (FAILED(hr))
		return false;
	

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * (MAX_LIGHTS_PER_TILE + 1) * 80 * 45);
	desc.StructureByteStride = static_cast<UINT>(sizeof(UINT) * (MAX_LIGHTS_PER_TILE + 1));

	hr = device->CreateBuffer(&desc, 0, &culledLightBuffer);
	if (FAILED(hr))
		return false;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = 80 * 45;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	device->CreateUnorderedAccessView(culledLightBuffer.Get(), &uavDesc, &culledLightBufferUAV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = 80 * 45;

	hr = device->CreateShaderResourceView(culledLightBuffer.Get(), &srvDesc, &culledLightBufferSRV);
	if (FAILED(hr))
		return false;

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;


	result = device->CreateRasterizerState(&rasterizerDesc, &rasterState);
	if (FAILED(result))
	{
		MessageBox(NULL, "Failed to create rasterizer state.",
			"D3D11 Initialisation Error", MB_OK);
		return false;
	}

	deviceContext->RSSetState(rasterState.Get());
	// Clear the blend state description.
	ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

	// Create an alpha enabled blend state description.
	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;


	// Create the blend state using the description.
	result = device->CreateBlendState(&blendStateDescription, &alphaEnableBlendingState);
	if (FAILED(result))
	{
		return false;
	}
	float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(alphaEnableBlendingState.Get(), blendFactor, 0xffffffff);

	this->device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(debug.GetAddressOf()));



	createShaders();
#ifdef _DEBUG
	debugger = new Debug(deviceContext.Get(), device.Get());
#endif

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); // (void)io;
	ImGui_ImplWin32_Init(this->window->handle);
	ImGui_ImplDX11_Init(this->device.Get(), this->deviceContext.Get());
	ImGui::StyleColorsDark();

	this->particleSystem.initiateParticles(device.Get(), deviceContext.Get(), L"ParticleUpdateCS.cso", L"ParticleCreateCS.cso", L"ParticleGS.cso");
	this->particleSystem2.initiateParticles(device.Get(), deviceContext.Get(), L"ParticleUpdateCS.cso", L"ParticleCreateCS.cso", L"ParticleGS.cso");

	/*for (int i = 0; i < 150; i++)
	{
		this->particleSystem.addParticle(1, 10, Vector3(0, 0, 3), Vector3(1, 0, 0), Vector4(1, 1, 0, 1), 0.1f);
		this->particleSystem.addParticle(1, 10, Vector3(0, 0, 3), Vector3(1, 0, 0), Vector4(1, 1, 0, 1), 0.1f);
		this->particleSystem.addParticle(1, 10, Vector3(0, 0, 3), Vector3(1, 0, 0), Vector4(1, 1, 0, 1), 0.1f);
		this->particleSystem.addParticle(1, 10, Vector3(0, 0, 3), Vector3(1, 0, 0), Vector4(1, 1, 0, 1), 0.1f);
	}*/
	this->particleSystem.addParticle(1, 2, Vector3(0, 0, 3), Vector3(1, 0, 0));
	this->particleSystem2.addParticle(1, 2, Vector3(0, 0, 3), Vector3(1, 0, 0));


	
	return true;
}

Debug* Graphics::getdebugger()
{
	return this->debugger;
}

Window* Graphics::getWindow()
{
	return this->window;
}

void Graphics::render(DynamicCamera* camera, float deltaTime)
{
	float color[4] = {
		0,0,0,1
	};
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Matrix view = camera->getViewMatrix().Transpose();
	deviceContext->Map(viewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &view, sizeof(Matrix));
	deviceContext->Unmap(viewProjBuffer.Get(), 0);
	deviceContext->CSSetConstantBuffers(1, 1, viewProjBuffer.GetAddressOf());
	fillLightBuffers();
	cullLights();

	Matrix viewProj = (camera->getViewMatrix() * camera->getProjectionMatrix()).Transpose();
	HRESULT hr = deviceContext->Map(viewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &viewProj, sizeof(Matrix));
	deviceContext->Unmap(viewProjBuffer.Get(), 0);


	this->particleSystem.updateParticles(deltaTime, viewProj);

	//deviceContext->OMSetRenderTargets(1, &renderTargetView, this->depthStencilView);

	this->particleSystem.drawAll(camera);

	this->particleSystem2.updateParticles(deltaTime, viewProj);

	this->particleSystem2.drawAll(camera);

	//set up Shaders
	deviceContext->IASetInputLayout(this->shaderDefault.vs.getInputLayout());
	deviceContext->PSSetShader(this->shaderDefault.ps.getShader(), nullptr, 0);
	deviceContext->VSSetShader(this->shaderDefault.vs.getShader(), nullptr, 0);
	deviceContext->VSSetConstantBuffers(0, 1, this->viewProjBuffer.GetAddressOf());
	deviceContext->PSSetSamplers(0, 1, this->sampler.GetAddressOf());
	deviceContext->PSSetShaderResources(1, 1, this->culledLightBufferSRV.GetAddressOf());
	deviceContext->PSSetConstantBuffers(2, 1, this->sunBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(1, 1, this->lightBuffer.GetAddressOf());

	Frustum frustum = camera->getFrustum();

	for (GameObject* object : drawableObjects)
	{
		if (Vector3::Distance(object->getPosition(), camera->getPosition()) < cullingDistance)
		{
			AABB boundingBox = object->getAABB();
			boundingBox = boundingBox.scale(object->getScale());
			boundingBox.maxPos += object->getPosition();
			boundingBox.minPos += object->getPosition();

			if (frustum.intersect(boundingBox, 10.0)) {
				SimpleMath::Matrix world = object->getTransform();
				SimpleMath::Matrix worldTr = DirectX::XMMatrixTranspose(world);
				D3D11_MAPPED_SUBRESOURCE mappedResource;
				HRESULT hr = deviceContext->Map(worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				CopyMemory(mappedResource.pData, &worldTr, sizeof(SimpleMath::Matrix));
				deviceContext->Unmap(worldBuffer.Get(), 0);
				UINT vertexCount = object->mesh->getVertexCount();
				UINT stride = sizeof(Vertex3D);
				UINT offset = 0;
				ID3D11ShaderResourceView* shaderResView;
				if (object->getTexture() != nullptr)
					shaderResView = object->getTexture()->getShaderResView();
				else
				{
					shaderResView = nullptr;
				}


				Vector4 modColor = object->getColor();
				hr = deviceContext->Map(colorBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
				CopyMemory(mappedResource.pData, &modColor, sizeof(Vector4));
				deviceContext->Unmap(colorBuffer.Get(), 0);

				deviceContext->VSSetConstantBuffers(1, 1, this->worldBuffer.GetAddressOf());
				deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				deviceContext->IASetVertexBuffers(0, 1, object->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
				deviceContext->PSSetShaderResources(0, 1, &shaderResView);
				deviceContext->PSSetConstantBuffers(0, 1, this->colorBuffer.GetAddressOf());

				deviceContext->Draw(vertexCount, 0);
			}
		}
	}

	deviceContext->IASetInputLayout(this->shaderDebug.vs.getInputLayout());
	deviceContext->PSSetShader(this->shaderDebug.ps.getShader(), nullptr, 0);
	deviceContext->VSSetShader(this->shaderDebug.vs.getShader(), nullptr, 0);
#if _DEBUG
	//debugger->DrawLine(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0 ), XMFLOAT3(1, 1, 0));
	//debugger->DrawCube(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0));
	//debugger->DrawRectangle(XMFLOAT3(0,0, 0), XMFLOAT3(1, 0, 0));
#endif

	// Present the back buffer to the screen since rendering is complete.
}

bool Graphics::createShaders()
{
	
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

	//2d shaders
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"SV_POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};


	UINT numElements = ARRAYSIZE(inputDesc);
	this->shaderDefault.createVS(device.Get(), shaderfolder + L"VertexShader.cso", inputDesc, numElements);
	this->shaderDefault.createPS(device.Get(), shaderfolder + L"PixelShader.cso");

	D3D11_INPUT_ELEMENT_DESC inputDesc2[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },	};
	
	UINT numElements2 = ARRAYSIZE(inputDesc2);
	this->shaderDebug.createVS(device.Get(), shaderfolder + L"DebugVs.cso", inputDesc2, numElements2);
	this->shaderDebug.createPS(device.Get(), shaderfolder + L"DebugPS.cso");



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
	// Create the texture sampler state.
	HRESULT result = device->CreateSamplerState(&samplerDesc, sampler.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}

	this->lightCullingShader.initialize(device.Get(), shaderfolder + L"ComputeLightCulling.cso");

	return true;
}

void Graphics::addParticle(Vector3 pos, Vector3 initialDirection, int nrOfParticles,  int lifeTime)
{
	Vector3 randomPos = 0.5f*Vector3(float(rand()), float(rand()), float(rand())) / RAND_MAX;
	randomPos += pos;
	float grey = float(rand()) / RAND_MAX;
	this->particleSystem.addParticle(nrOfParticles, lifeTime, randomPos, initialDirection);
}

void Graphics::addParticle2(Vector3 pos, Vector3 initialDirection, int nrOfParticles, int lifeTime)
{
	Vector3 randomPos = 0.5f * Vector3(float(rand()), float(rand()), float(rand())) / RAND_MAX;
	randomPos += pos;
	float grey = float(rand()) / RAND_MAX;
	this->particleSystem2.addParticle(nrOfParticles, lifeTime, randomPos, initialDirection);
}

void Graphics::setParticleColorNSize(Vector4 colors[4], int nrOfColors, float startSize, float endSize)
{
	if (nrOfColors < 5)
	{
		this->particleSystem.changeColornSize(colors, nrOfColors, startSize, endSize);
	}
}

void Graphics::setParticle2ColorNSize(Vector4 colors[4], int nrOfColors, float startSize, float endSize)
{
	if (nrOfColors < 5)
	{
		this->particleSystem2.changeColornSize(colors, nrOfColors, startSize, endSize);
	}
}

void Graphics::clearScreen()
{
	float color[4] = { 0,0,0,1 };
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);
}

ID3D11DeviceContext* Graphics::getDeviceContext()
{
	return this->deviceContext.Get();
}

ID3D11Device* Graphics::getDevice()
{
	return this->device.Get();
}

void Graphics::loadMesh(std::string fileName)
{
	Mesh newMesh;
	if (meshes.find(fileName) == meshes.end())
	{
		
		Importer imp;
		if (imp.loadMesh(fileName.c_str()))
		{
			meshes[fileName] = newMesh;
			
			std::vector<Vertex3D> tempVec;
			Vertex3D vertex;
			Vertex* vertices = imp.getVertices();
			for (int i = 0; i < imp.getVertexCount(); i++)
			{
				vertex.position.x = vertices[i].x;
				vertex.position.y = vertices[i].y;
				vertex.position.z = vertices[i].z;

					vertex.normal.x = vertices[i].nx;
					vertex.normal.y = vertices[i].ny;
					vertex.normal.z = vertices[i].nz;

					vertex.uv.x = vertices[i].u;
					vertex.uv.y = vertices[i].v;

					tempVec.push_back(vertex);
				
			}

			meshes[fileName].insertDataToMesh(tempVec);
			AABB aabb;
			imp.getMaxBBox(aabb.maxPos.x, aabb.maxPos.y, aabb.maxPos.z);
			imp.getMinBBox(aabb.minPos.x, aabb.minPos.y, aabb.minPos.z);
			meshes[fileName].setAABB(aabb);
			int bufferSize = static_cast<int>(meshes[fileName].vertices.size()) * sizeof(Vertex3D);
			UINT stride = sizeof(Vertex3D);

			D3D11_BUFFER_DESC vBufferDesc;
			ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));

			vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vBufferDesc.ByteWidth = bufferSize;
			vBufferDesc.CPUAccessFlags = 0;
			vBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA subData;
			ZeroMemory(&subData, sizeof(subData));
			subData.pSysMem = meshes[fileName].vertices.data();

			HRESULT hr = device->CreateBuffer(&vBufferDesc, &subData, meshes[fileName].vertexBuffer.GetAddressOf());
			//meshes[fileName].vertices.clear();//Either save vertex data or not. Depends if we want to use it for picking or something else
		}
	}
}

void Graphics::loadModel(std::string path)
{
   std::string modelDir {MODEL_ROOT_DIR};
               modelDir += path;
	loadMesh( modelDir+"/mesh.bin" );
	loadTexture( modelDir+"/_diffuse.tga", true );
   // TODO: load other texture channels
}

void Graphics::loadShape(Shapes shape, Vector3 normalForQuad)
{
	Mesh newMesh;
	std::string fileName;
	switch (shape)
	{
	case SHAPE_CUBE:
		fileName = "Cube";
		if (meshes.find(fileName) == meshes.end())
		{
			std::vector<Vertex3D> vecTemp;
			meshes[fileName] = newMesh;
			Vertex3D vec[] = {
				{Vector3(-1.0f,  1.0f, -1.0f), Vector2(0.0f, 0.0f), Vector3(0.0f,0.0f,-1.0f)},
				{Vector3(1.0f,1.0f,-1.0f), Vector2(1.0f, 0.0f), Vector3(0.0f,0.0f,-1.0f)},
				{Vector3(-1.0f,-1.0f,-1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f,0.0f,-1.0f)},
				{Vector3(-1.0f,-1.0f,-1.0f), Vector2(0.0f, 1.0f), Vector3(0.0f,0.0f,-1.0f)},
				{Vector3(1.0f,1.0f,-1.0f), Vector2(1.0f,0.0f), Vector3(0.0f,0.0f,-1.0f)},
				{Vector3(1.0f,-1.0f,-1.0f), Vector2(1.0f,1.0f), Vector3(0.0f,0.0f,-1.0f)},

				{Vector3(1.0f,1.0f,-1.0f), Vector2(0.0f,0.0f), Vector3(1.0f,0.0f,0.0f)},
				{Vector3(1.0f,1.0f,1.0f), Vector2(1.0f,0.0f), Vector3(1.0f,0.0f,0.0f)},
				{Vector3(1.0f, -1.0f, -1.0f) ,Vector2(0.0f, 1.0f)	,Vector3(1.0f,  0.0f,  0.0f)},
				{Vector3(1.0f, -1.0f, -1.0f) , Vector2(0.0f, 1.0f)  ,Vector3(1.0f,  0.0f,  0.0f)},
				{Vector3(1.0f,  1.0f,  1.0f) , Vector2(1.0f, 0.0f)  ,Vector3(1.0f,  0.0f,  0.0f)},
				{Vector3(1.0f, -1.0f,  1.0f), Vector2(1.0f, 1.0f)	,Vector3(1.0f,  0.0f,  0.0f)},

				{Vector3(1.0f,  1.0f,  1.0f),Vector2(0.0f, 0.0f)  ,Vector3(0.0f,  0.0f,  1.0f)},
				{Vector3(-1.0f, 1.0f,  1.0f),Vector2(1.0f, 0.0f)  ,Vector3(0.0f,  0.0f,  1.0f)},
				{Vector3(1.0f, -1.0f,  1.0f),Vector2(0.0f, 1.0f)  ,Vector3(0.0f,  0.0f,  1.0f)},
				{Vector3(1.0f, -1.0f,  1.0f),Vector2(0.0f, 1.0f)  ,Vector3(0.0f,  0.0f,  1.0f)},
				{Vector3(-1.0f, 1.0f,  1.0f),Vector2(1.0f, 0.0f)  ,Vector3(0.0f,  0.0f,  1.0f)},
				{Vector3(-1.0f,-1.0f,  1.0f),Vector2(1.0f, 1.0f)  ,Vector3(0.0f,  0.0f,  1.0f)},

				{Vector3(-1.0f, 1.0f,  1.0f),Vector2(0.0f, 0.0f)	,Vector3(-1.0f,  0.0f,  0.0f)},
				{Vector3(-1.0f, 1.0f, -1.0f),Vector2(1.0f, 0.0f)	,Vector3(-1.0f,  0.0f,  0.0f)},
				{Vector3(-1.0f,-1.0f,  1.0f),Vector2(0.0f, 1.0f)	,Vector3(-1.0f,  0.0f,  0.0f)},
				{Vector3(-1.0f,-1.0f,  1.0f),Vector2(0.0f, 1.0f)	,Vector3(-1.0f,  0.0f,  0.0f)},
				{Vector3(-1.0f, 1.0f, -1.0f),Vector2(1.0f, 0.0f)	,Vector3(-1.0f,  0.0f,  0.0f)},
				{Vector3(-1.0f,-1.0f, -1.0f),Vector2(1.0f, 1.0f)	,Vector3(-1.0f,  0.0f,  0.0f)},

				{Vector3(-1.0f, 1.0f,  1.0f),Vector2(0.0f, 0.0f) ,Vector3(0.0f,  1.0f  ,0.0f)},
				{Vector3(1.0f,  1.0f,  1.0f),Vector2(1.0f, 0.0f) ,Vector3(0.0f,  1.0f  ,0.0f)},
				{Vector3(-1.0f, 1.0f, -1.0f),Vector2(0.0f, 1.0f)	,Vector3(0.0f,  1.0f  ,0.0f)},
				{Vector3(-1.0f, 1.0f, -1.0f),Vector2(0.0f, 1.0f),Vector3(0.0f,  1.0f  ,0.0f)},
				{Vector3(1.0f,  1.0f,  1.0f),Vector2(1.0f, 0.0f) ,Vector3(0.0f,  1.0f  ,0.0f)},
				{Vector3(1.0f,  1.0f, -1.0f),Vector2(1.0f, 1.0f) ,Vector3(0.0f,  1.0f  ,0.0f)},

				{Vector3(-1.0f, -1.0f, -1.0f),Vector2(0.0f, 0.0f),Vector3(0.0f, -1.0f , 0.0f)},
				{Vector3(1.0f, -1.0f,-1.0f),Vector2(1.0f, 0.0f),Vector3(0.0f, -1.0f , 0.0f)},
				{Vector3(-1.0f, -1.0f,  1.0f),Vector2(0.0f, 1.0f),Vector3(0.0f, -1.0f , 0.0f)},
				{Vector3(-1.0f, -1.0f,  1.0f),Vector2(0.0f, 1.0f),Vector3(0.0f, -1.0f , 0.0f)},
				{Vector3(1.0f, -1.0f, -1.0f),Vector2(1.0f, 0.0f),Vector3(0.0f, -1.0f , 0.0f)},
				{Vector3(1.0f, -1.0f,  1.0f),Vector2(1.0f, 1.0f),Vector3(0.0f, -1.0f , 0.0f)}
			};

			for (int i = 0; i < 36; i++)
			{
				vecTemp.push_back(vec[i]);
			}


			meshes[fileName].insertDataToMesh(vecTemp);
			AABB boundingBox;
			boundingBox.minPos = Vector3(-1.0, -1.0, -1.0);
			boundingBox.maxPos = Vector3(1.0, 1.0, 1.0);
			meshes[fileName].setAABB(boundingBox);

			int bufferSize = static_cast<int>(meshes[fileName].vertices.size()) * sizeof(Vertex3D);
			UINT stride = sizeof(Vertex3D);

			D3D11_BUFFER_DESC vBufferDesc;
			ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));

			vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vBufferDesc.ByteWidth = bufferSize;
			vBufferDesc.CPUAccessFlags = 0;
			vBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA subData;
			ZeroMemory(&subData, sizeof(subData));
			subData.pSysMem = meshes[fileName].vertices.data();

			HRESULT hr = device->CreateBuffer(&vBufferDesc, &subData, meshes[fileName].vertexBuffer.GetAddressOf());
			meshes[fileName].vertices.clear();
		}
		break;
	case SHAPE_QUAD:
		fileName = "Quad";
		if (meshes.find(fileName) == meshes.end())
		{
			std::vector<Vertex3D> vecTemp;
			Vertex3D vertex;
			meshes[fileName] = newMesh;
			vertex.position = Vector3(-1.0, 0.0, -1.0);
			vertex.uv = Vector2(0.0, 1.0);
			vertex.normal = Vector3(0.0, 1.0, 0.0);
			vecTemp.push_back(vertex);

			vertex.position = Vector3(-1.0, 0.0, 1.0);
			vertex.uv = Vector2(0.0, 0.0);
			vecTemp.push_back(vertex);

			vertex.position = Vector3(1.0, 0.0, -1.0);
			vertex.uv = Vector2(1.0, 1.0);
			vecTemp.push_back(vertex);

			vertex.position = Vector3(-1.0, 0.0, 1.0);
			vertex.uv = Vector2(0.0, 0.0);
			vecTemp.push_back(vertex);

			vertex.position = Vector3(1.0, 0.0, 1.0);
			vertex.uv = Vector2(1.0, 0.0);
			vecTemp.push_back(vertex);

			vertex.position = Vector3(1.0, 0.0, -1.0);
			vertex.uv = Vector2(1.0, 1.0);
			vecTemp.push_back(vertex);

			meshes[fileName].insertDataToMesh(vecTemp);
			AABB boundingBox;
			boundingBox.minPos = Vector3(-1.0f, -0.01f, -1.0f);
			boundingBox.maxPos = Vector3(1.0f, 0.01f, 1.0f);
			meshes[fileName].setAABB(boundingBox);

			int bufferSize = static_cast<int>(meshes[fileName].vertices.size()) * sizeof(Vertex3D);
			UINT stride = sizeof(Vertex3D);

			D3D11_BUFFER_DESC vBufferDesc;
			ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));

			vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vBufferDesc.ByteWidth = bufferSize;
			vBufferDesc.CPUAccessFlags = 0;
			vBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA subData;
			ZeroMemory(&subData, sizeof(subData));
			subData.pSysMem = meshes[fileName].vertices.data();

			HRESULT hr = device->CreateBuffer(&vBufferDesc, &subData, meshes[fileName].vertexBuffer.GetAddressOf());
			meshes[fileName].vertices.clear();
		}
		break;
	default:
		break;
	}
}

bool Graphics::loadTexture(std::string path, bool overridePath )
{
   std::string texturePath;
   if (!overridePath) {
      texturePath += TEXTURE_ROOT_DIR;
      texturePath += path;
      texturePath += ".tga";
   } else texturePath = path;

	if (textures.find(texturePath) == textures.end()) {
		Texture* newTexture = new Texture();

		if (!newTexture->Initialize(this->device.Get(), this->deviceContext.Get(), texturePath.c_str(), -1)) {
			delete newTexture;
			return false;
		}
		textures[texturePath] = newTexture;
	}
	return true;
}

const Mesh* Graphics::getMeshPointer(const char* localPath)
{  // TEMP! TODO: add separate function for primitives (e.g. "Cube")
   std::string meshPath;
   if ( localPath != "Cube" ) {
      meshPath = MODEL_ROOT_DIR;
      meshPath += localPath;
      meshPath += "/mesh.bin";
   }
   else meshPath = std::string(localPath);

	if ( meshes.find(meshPath) == meshes.end() ) return nullptr;
	else return &meshes[meshPath];
}

Texture* Graphics::getTexturePointer(const char* path, bool isModel )
{  // TEMP! TODO: add separate function for primitives (e.g. "Cube")
   std::string texturePath;
   if (!isModel) {
      texturePath += TEXTURE_ROOT_DIR;
      texturePath += path;
      texturePath += ".tga";
   } else texturePath = MODEL_ROOT_DIR + std::string(path) + "/_diffuse.tga";

	if (textures.find(texturePath) == textures.end()) {
      assert(false && "Failed to load texture!" );
		return nullptr;
   }
	else return textures[texturePath];
}

void Graphics::addToDraw(GameObject* o)
{
   assert( o != nullptr );
	drawableObjects.push_back(o);
}

void Graphics::removeFromDraw(GameObject* o)
{
	auto obj = std::find(drawableObjects.begin(), drawableObjects.end(), o);

	if (obj != drawableObjects.end())
	{
		drawableObjects.erase(obj);
	}
}

void Graphics::clearDraw()
{
	drawableObjects.clear();
}

void Graphics::setLightList(LightList* lightList)
{
	this->lightList = lightList;
	fillLightBuffers();
}

void Graphics::presentScene()
{
	swapChain->Present(0, 0);
}

void Graphics::fillLightBuffers()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(lightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &lightList->lights, sizeof(Light) * lightList->maxSize);
	deviceContext->Unmap(lightBuffer.Get(), 0);

	hr = deviceContext->Map(sunBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &lightList->sun, sizeof(Sun));
	deviceContext->Unmap(sunBuffer.Get(), 0);
}

void Graphics::cullLights()
{
	deviceContext->CSSetShader(lightCullingShader.getShader(), NULL, 0);
	ID3D11ShaderResourceView* nullSRV = NULL;
	deviceContext->PSSetShaderResources(1, 1, &nullSRV);
	deviceContext->CSSetConstantBuffers(0, 1, lightBuffer.GetAddressOf());
	deviceContext->CSSetShaderResources(0, 1, frustumBufferSRV.GetAddressOf());
	const UINT uavCounter = 0;
	deviceContext->CSSetUnorderedAccessViews(0, 1, culledLightBufferUAV.GetAddressOf(), &uavCounter);

	deviceContext->Dispatch(80, 45, 1);

	ID3D11UnorderedAccessView* nullUAV = NULL;
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, 0);
}

HRESULT Graphics::createFrustumBuffer(DynamicCamera* camera)
{
	struct PartialFrustum {
		Vector4 left;
		Vector4 right;
		Vector4 top;
		Vector4 bottom;
	} frustum;

	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.ByteWidth = static_cast<UINT>(sizeof(PartialFrustum) * 80 * 45);
	desc.StructureByteStride = sizeof(PartialFrustum);

	PartialFrustum* frustumTiles = new PartialFrustum[80 * 45];
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = frustumTiles;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	Vector3 eye = Vector3(0.0, 0.0, 0.0);
	
	Matrix invProj = camera->getProjectionMatrix().Invert();

	auto clipToView = [&](Vector4 clip) {
		Vector4 view = Vector4::Transform(clip, invProj);
		view = view / view.w;
		return view;
	};

	auto screenToView = [&](Vector4 screen) {
		Vector2 texCoord = Vector2(screen) / Vector2(1280, 720);
		texCoord.y = 1.f - texCoord.y;
		texCoord = texCoord * Vector2(2.0f, 2.0f) - Vector2(1.0f, 1.0f);
		Vector4 clip = Vector4(texCoord.x, texCoord.y, screen.z, screen.w);

		return clipToView(clip);
	};

	for (int y = 0; y < 45; ++y)
	{
		for (int x = 0; x < 80; ++x)
		{
			Vector4 screenTopLeft     =	Vector4(x * 16.f, y * 16.f, 1.0f, 1.0f);
			Vector4 screenTopRight    =	Vector4((x + 1) * 16.f, y * 16.f, 1.0f, 1.0f);
			Vector4 screenBottomLeft  =	Vector4(x * 16.f, (y + 1) * 16.f, 1.0f, 1.0f);
			Vector4 screenBottomRight = Vector4((x + 1) * 16.f, (y + 1) * 16.f, 1.0f, 1.0f);

			Vector3 viewTopLeft     = Vector3(screenToView(screenTopLeft));
			Vector3 viewTopRight    = Vector3(screenToView(screenTopRight));
			Vector3 viewBottomLeft  = Vector3(screenToView(screenBottomLeft));
			Vector3 viewBottomRight = Vector3(screenToView(screenBottomRight));

			frustum.top    = DirectX::XMPlaneFromPoints(eye, viewTopLeft, viewTopRight);
			frustum.bottom = DirectX::XMPlaneFromPoints(eye, viewBottomRight, viewBottomLeft);
			frustum.left   = DirectX::XMPlaneFromPoints(eye, viewBottomLeft, viewTopLeft);
			frustum.right  = DirectX::XMPlaneFromPoints(eye, viewTopRight, viewBottomRight);

			frustumTiles[y * 80 + x] = frustum;
		}
	}

	HRESULT hr = device->CreateBuffer(&desc, &data, frustumBuffer.GetAddressOf());
	delete[] frustumTiles;
	
	if (FAILED(hr))
		return hr;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = 80 * 45;

	hr = device->CreateShaderResourceView(frustumBuffer.Get(), &srvDesc, frustumBufferSRV.GetAddressOf());

	return hr;
}

void Graphics::setCullingDistance(float dist)
{
	cullingDistance = dist;
}

float Graphics::getCullingDistance()
{
	return cullingDistance;
}
