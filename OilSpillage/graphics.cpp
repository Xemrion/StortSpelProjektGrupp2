#include "graphics.h"
#include <algorithm>
#include"Importer/importerClass.h"
#include<cstring>

Graphics::Graphics()
{
	this->window = nullptr;
	this->swapChain = nullptr;
	this->device = nullptr;
	this->deviceContext = nullptr;
	this->vp = {};

	this->renderTargetView = nullptr;
	this->depthStencilBuffer = nullptr;
	this->depthStencilState = nullptr;
	this->depthStencilView = nullptr;
	this->rasterState = nullptr;
	this->alphaEnableBlendingState = nullptr;
	this->viewProjBuffer = nullptr;
	this->worldBuffer = nullptr;
	this->colorBuffer = nullptr;

	this->sampler = nullptr;

	this->debugger = nullptr;
	this->debug = nullptr;
}

Graphics::~Graphics()
{
	this->depthStencilBuffer->Release();
	this->depthStencilView->Release();
	this->depthStencilState->Release();
	this->alphaEnableBlendingState->Release();
	this->rasterState->Release();
	this->renderTargetView->Release();

	this->viewProjBuffer->Release();
	this->worldBuffer->Release();
	this->colorBuffer->Release();

	this->sampler->Release();

	if (this->swapChain)
		this->swapChain->Release();
	if (this->deviceContext)
		this->deviceContext->Release();
	if(debug!=nullptr)
		debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
	if (this->device)
		this->device->Release();
	if (this->debug)
		this->debug->Release();

	delete this->debugger;

	for (auto i = textures.begin(); i != textures.end(); i++)
	{
		delete i->second;
	}
}

bool Graphics::init(Window* window, float fov, Camera theCamera)
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
		&swapChain,
		&device,
		NULL,
		&deviceContext);


		// get the address of the back buffer
		ID3D11Texture2D* backBufferPtr = nullptr;
		swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)& backBufferPtr);
		if (FAILED(result) || backBufferPtr == nullptr)
		{
			MessageBox(this->window->handle, "Could not ID3D11Texture2D* backBufferPtr", "Error", MB_OK); //L"", L"", ;
			return false;
		}

		// use the back buffer address to create the render target
		device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
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
		descDepth.Format = DXGI_FORMAT_R24G8_TYPELESS;
		descDepth.SampleDesc.Count = 4;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;


		D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
		dsv_desc.Flags = 0;
		dsv_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
		dsv_desc.Texture2D.MipSlice = 0;
		D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
		sr_desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		sr_desc.Texture2D.MostDetailedMip = 0;
		sr_desc.Texture2D.MipLevels = 1;
		result = device->CreateTexture2D(&descDepth, NULL, &depthStencilBuffer);
		if (FAILED(result))
		{
			return false;
		}
		result = device->CreateDepthStencilView(depthStencilBuffer, &dsv_desc, &depthStencilView);
		if (FAILED(result))
		{
			return false;
		}
		deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
		result = device->CreateShaderResourceView(depthStencilBuffer, &sr_desc, &this->depthSRV);
		//the depth Stencil State
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;

		ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		// Depth test parameters
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;

		// Create depth stencil state
		result = device->CreateDepthStencilState(&depthStencilDesc, &depthStencilState);
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
	desc.ByteWidth = static_cast<UINT>((sizeof(Vector4) * 2) * maxPointLights + sizeof(Vector4));
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, &lightBuffer);
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

	deviceContext->RSSetState(rasterState);
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
	deviceContext->OMSetBlendState(alphaEnableBlendingState, blendFactor, 0xffffffff);

#if _DEBUG
	this->device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debug));
#endif
	


	createShaders();
	debugger = new Debug(deviceContext, device, theCamera);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); // (void)io;
	ImGui_ImplWin32_Init(this->window->handle);
	ImGui_ImplDX11_Init(this->device, this->deviceContext);
	ImGui::StyleColorsDark();

	this->particleSystem.initiateParticles(device, deviceContext,this->depthSRV);
	/*for (int i = 0; i < 150; i++)
	{
		this->particleSystem.addParticle(1, 10, Vector3(0, 0, 3), Vector3(1, 0, 0), Vector4(1, 1, 0, 1), 0.1f);
		this->particleSystem.addParticle(1, 10, Vector3(0, 0, 3), Vector3(1, 0, 0), Vector4(1, 1, 0, 1), 0.1f);
		this->particleSystem.addParticle(1, 10, Vector3(0, 0, 3), Vector3(1, 0, 0), Vector4(1, 1, 0, 1), 0.1f);
		this->particleSystem.addParticle(1, 10, Vector3(0, 0, 3), Vector3(1, 0, 0), Vector4(1, 1, 0, 1), 0.1f);
	}*/
	this->particleSystem.addParticle(1, 2, Vector3(0, 0, 3), Vector3(1, 0, 0));
	

	
	return true;
}

Debug* Graphics::getdebugger()
{
	return this->debugger;
}

void Graphics::render(Camera camera, float deltaTime)
{
	float color[4] = {
		0.5,0.5,0.0,1
	};
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);
	deviceContext->OMSetDepthStencilState(this->depthStencilState, 0);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Matrix viewProj = (camera.getViewMatrix() * camera.getProjectionMatrix()).Transpose();
	HRESULT hr = deviceContext->Map(viewProjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &viewProj, sizeof(Matrix));
	deviceContext->Unmap(viewProjBuffer, 0);

	hr = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	void* dataPtr = mappedResource.pData;
	ZeroMemory(dataPtr, sizeof(PointLight) * maxPointLights);
	CopyMemory(dataPtr, &sunVector, sizeof(Vector4));
	dataPtr = (void*)((size_t)dataPtr + sizeof(Vector4));
	CopyMemory(dataPtr, pointLights.data(), min(maxPointLights, pointLights.size()) * sizeof(PointLight));
	deviceContext->Unmap(lightBuffer, 0);

	//set up Shaders
	deviceContext->IASetInputLayout(this->shaderDefault.vs.GetInputLayout());
	deviceContext->PSSetShader(this->shaderDefault.ps.GetShader(), nullptr, 0);
	deviceContext->VSSetShader(this->shaderDefault.vs.GetShader(), nullptr, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &this->viewProjBuffer);
	deviceContext->PSSetSamplers(0, 1, &this->sampler);
	for (GameObject* object : drawableObjects)
	{

		SimpleMath::Matrix world = object->getTransform();
		SimpleMath::Matrix worldTr = DirectX::XMMatrixTranspose(world);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(worldBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &worldTr, sizeof(SimpleMath::Matrix));
		deviceContext->Unmap(worldBuffer, 0);
		UINT vertexCount = object->mesh->getVertexCount();
		UINT stride = sizeof(Vertex3D);
		UINT offset = 0;
		ID3D11ShaderResourceView* shaderResView;
		if(object->getTexture()!=nullptr)
			shaderResView = object->getTexture()->getShaderResView();
		else
		{
			shaderResView = nullptr;
		}

		
		Vector4 modColor = object->getColor();
		hr = deviceContext->Map(colorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &modColor, sizeof(Vector4));
		deviceContext->Unmap(colorBuffer, 0);

		deviceContext->VSSetConstantBuffers(1, 1, &this->worldBuffer);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetVertexBuffers(0, 1, &object->mesh->vertexBuffer, &stride, &offset);
		deviceContext->PSSetShaderResources(0, 1, &shaderResView);
		deviceContext->PSSetConstantBuffers(0, 1, &this->colorBuffer);
		deviceContext->PSSetConstantBuffers(2, 1, &this->lightBuffer);
		deviceContext->Draw(vertexCount, 0);
	}
	ID3D11DepthStencilView* nDPV = nullptr;
	deviceContext->OMSetRenderTargets(1, &renderTargetView, nDPV);

	this->particleSystem.updateParticles(deltaTime,viewProj);
	deviceContext->OMSetRenderTargets(1, &renderTargetView, this->depthStencilView);

	this->particleSystem.drawAll(camera);
	//this->particleSystem.addParticle(1, 10, Vector3(float(rand()), float(rand()), float(rand()))/RAND_MAX, Vector3(float(rand()), float(rand()), float(rand()))/RAND_MAX, Vector4(float(rand())/RAND_MAX, float(rand())/RAND_MAX, 0.0f,0.5f), 0.1f);
	


	deviceContext->IASetInputLayout(this->shaderDebug.vs.GetInputLayout());
	deviceContext->PSSetShader(this->shaderDebug.ps.GetShader(), nullptr, 0);
	deviceContext->VSSetShader(this->shaderDebug.vs.GetShader(), nullptr, 0);

	/*debugger->DrawLine(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0 ), XMFLOAT3(1, 1, 0));
	debugger->DrawCube(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0));*/
	//debugger->DrawRectangle(XMFLOAT3(0,0, 0), XMFLOAT3(1, 0, 0));
	
	// Present the back buffer to the screen since rendering is complete.
}

bool Graphics::createShaders()
{
	
	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
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
	}
	else
	{
		shaderfolder = L"..\\x64\\Debug\\";
	}

	//2d shaders
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"SV_POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};


	UINT numElements = ARRAYSIZE(inputDesc);
	this->shaderDefault.createVS(device, shaderfolder + L"VertexShader.cso", inputDesc, numElements);
	this->shaderDefault.createPS(device, shaderfolder + L"PixelShader.cso");

	D3D11_INPUT_ELEMENT_DESC inputDesc2[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },	};
	
	UINT numElements2 = ARRAYSIZE(inputDesc2);
	this->shaderDebug.createVS(device, shaderfolder + L"DebugVs.cso", inputDesc2, numElements2);
	this->shaderDebug.createPS(device, shaderfolder + L"DebugPS.cso");



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
	HRESULT result = device->CreateSamplerState(&samplerDesc, &sampler);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void Graphics::addParticle(Vector3 pos, Vector3 initialDirection, int nrOfParticles,  int lifeTime)
{
	Vector3 randomPos = 0.005f*Vector3(float(rand()), float(rand()), float(rand())) / RAND_MAX;
	randomPos += pos;
	float grey = float(rand()) / RAND_MAX;
	this->particleSystem.addParticle(nrOfParticles, lifeTime, randomPos, initialDirection);
}

void Graphics::setParticleColorNSize(Vector4 colors[4], int nrOfColors, float startSize, float endSize)
{
	if (nrOfColors < 5)
	{
		this->particleSystem.changeColornSize(colors, nrOfColors, startSize, endSize);
	}
}

void Graphics::loadMesh(std::string fileName,std::string meshName)
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
			for (int i = 0; i < imp.getMeshCount(); i++)
			{
				Vertex* vertices = imp.getVertices(i);
				for (int j = 0; j < imp.getVertexCount(i); j++)
				{
					vertex.position.x = vertices[j].x;
					vertex.position.y = vertices[j].y;
					vertex.position.z = vertices[j].z;

					vertex.normal.x = vertices[j].nx;
					vertex.normal.y = vertices[j].ny;
					vertex.normal.z = vertices[j].nz;

					vertex.uv.x = vertices[j].u;
					vertex.uv.y = vertices[j].v;

					tempVec.push_back(vertex);
				}
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

			HRESULT hr = device->CreateBuffer(&vBufferDesc, &subData, &meshes[fileName].vertexBuffer);
			//meshes[fileName].vertices.clear();//Either save vertex data or not. Depends if we want to use it for picking or something else
		}
	}
}
void Graphics::loadModel(std::string fileName,std::string modelName)
{
	//sadasde
	this->loadMesh(fileName + ".bin","temp");
	this->loadTexture(fileName + ".tga","temp");
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

			HRESULT hr = device->CreateBuffer(&vBufferDesc, &subData, &meshes[fileName].vertexBuffer);
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

			HRESULT hr = device->CreateBuffer(&vBufferDesc, &subData, &meshes[fileName].vertexBuffer);
			meshes[fileName].vertices.clear();
		}
		break;
	default:
		break;
	}
}

bool Graphics::loadTexture(std::string fileName, std::string textureName)
{
	if (textures.find(fileName) == textures.end())
	{
		Texture* newTexture = new Texture();

		if (!newTexture->Initialize(this->device, this->deviceContext, fileName.c_str(), -1))
		{
			delete newTexture;
			return false;
		}

		textures[fileName] = newTexture;
	}

	return true;
}

const Mesh* Graphics::getMeshPointer(const char* fileName)
{

	if (meshes.find(fileName) == meshes.end())
	{
		return nullptr;
	}
	return &meshes[fileName];
}

Texture* Graphics::getTexturePointer(const char* fileName)
{
	if (textures.find(fileName) == textures.end())
	{
		return nullptr;
	}

	return textures[fileName];
}

ID3D11ShaderResourceView* Graphics::getDepthSRV()
{
	return this->depthSRV;
}

void Graphics::addToDraw(GameObject* o)
{
	drawableObjects.push_back(o);
}

void Graphics::removeFromDraw(GameObject* o)
{
	std::find(drawableObjects.begin(), drawableObjects.end(), o);
}

void Graphics::addPointLight(PointLight light)
{
	pointLights.push_back(light);
}

void Graphics::clearPointLights()
{
	pointLights.clear();
}

void Graphics::setSunVector(Vector3 vectorToSun)
{
	vectorToSun.Normalize();
	sunVector = Vector4(vectorToSun.x, vectorToSun.y, vectorToSun.z, 0.0);
}

Vector3 Graphics::getSunVector()
{
	return Vector3(sunVector);
}

void Graphics::presentScene()
{
	swapChain->Present(1, 0);
}

