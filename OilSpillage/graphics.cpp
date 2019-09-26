#include "graphics.h"
#include <algorithm>
#include"Importer/importerClass.h"
#include<cstring>
#include "ShaderDefines.hlsli"

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
	this->lightBuffer = nullptr;
	this->frustumBuffer = nullptr;
	this->culledLightBuffer = nullptr;
	this->lightAppendBufferView = nullptr;
	this->culledLightBufferView = nullptr;
	this->lightCountBuffer = nullptr;

	this->sampler = nullptr;

	this->debugger = nullptr;
	this->debug = nullptr;

	lightBufferContents = new LightBufferContents;
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
	this->lightBuffer->Release();
	this->frustumBuffer->Release();
	this->culledLightBuffer->Release();
	this->lightCountBuffer->Release();

	this->lightAppendBufferView->Release();
	this->culledLightBufferView->Release();

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
		&swapChain,
		&device,
		NULL,
		&deviceContext);

	if (SUCCEEDED(result))
	{
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
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_R32_TYPELESS;
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
		result = device->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
		if (FAILED(result))
		{
			return false;
		}
		deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);

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

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(Frustum));
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, &frustumBuffer);
	if (FAILED(hr))
		return false;

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.ByteWidth = static_cast<UINT>(sizeof(SpotLight) * (MAX_LIGHTS_ON_SCREEN));
	desc.StructureByteStride = static_cast<UINT>(sizeof(SpotLight));

	hr = device->CreateBuffer(&desc, 0, &culledLightBuffer);
	if (FAILED(hr))
		return false;

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.ByteWidth = 16;
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, &lightCountBuffer);
	if (FAILED(hr))
		return false;

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = MAX_LIGHTS_ON_SCREEN;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	device->CreateUnorderedAccessView(culledLightBuffer, &uavDesc, &lightAppendBufferView);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = (UINT)MAX_LIGHTS_ON_SCREEN;

	hr = device->CreateShaderResourceView(culledLightBuffer, &srvDesc, &culledLightBufferView);
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
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
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

	this->device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debug));


	createShaders();
	debugger = new Debug(deviceContext, device);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); // (void)io;
	ImGui_ImplWin32_Init(this->window->handle);
	ImGui_ImplDX11_Init(this->device, this->deviceContext);
	ImGui::StyleColorsDark();

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

void Graphics::render(DynamicCamera* camera)
{
	float color[4] = {
		0,0,0,1
	};
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);
	deviceContext->OMSetDepthStencilState(this->depthStencilState, 0);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Matrix viewProj = (camera->getViewMatrix() * camera->getProjectionMatrix()).Transpose();
	HRESULT hr = deviceContext->Map(viewProjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &viewProj, sizeof(Matrix));
	deviceContext->Unmap(viewProjBuffer, 0);

	fillLightBuffers(camera->getFrustum());

	//set up Shaders
	deviceContext->IASetInputLayout(this->shaderDefault.vs.GetInputLayout());
	deviceContext->PSSetShader(this->shaderDefault.ps.GetShader(), nullptr, 0);
	deviceContext->VSSetShader(this->shaderDefault.vs.GetShader(), nullptr, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &this->viewProjBuffer);
	deviceContext->PSSetSamplers(0, 1, &this->sampler);
	deviceContext->PSSetShaderResources(1, 1, &this->culledLightBufferView);
	deviceContext->PSSetConstantBuffers(2, 1, &this->sunBuffer);
	deviceContext->PSSetConstantBuffers(3, 1, &this->lightCountBuffer);

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

		deviceContext->Draw(vertexCount, 0);
	}

	deviceContext->IASetInputLayout(this->shaderDebug.vs.GetInputLayout());
	deviceContext->PSSetShader(this->shaderDebug.ps.GetShader(), nullptr, 0);
	deviceContext->VSSetShader(this->shaderDebug.vs.GetShader(), nullptr, 0);

	debugger->DrawLine(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 1, 0 ), XMFLOAT3(1, 1, 0));
	debugger->DrawCube(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0));
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

	this->lightCullingShader.initialize(device, shaderfolder + L"ComputeLightCulling.cso");

	return true;
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
			Vertex* vertices = imp.getVertices();
			std::vector<Vertex3D> tempVec;
			Vertex3D vertex;
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

			HRESULT hr = device->CreateBuffer(&vBufferDesc, &subData, &meshes[fileName].vertexBuffer);
			//meshes[fileName].vertices.clear();//Either save vertex data or not. Depends if we want to use it for picking or something else
		}
	}
}

void Graphics::loadModel(std::string fileName)
{
	this->loadMesh(fileName + ".bin");
	this->loadTexture(fileName + ".tga");
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

bool Graphics::loadTexture(std::string fileName)
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

void Graphics::addToDraw(GameObject* o)
{
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

void Graphics::setLightList(LightList* lightList)
{
	this->lightList = lightList;
}

void Graphics::presentScene()
{
	swapChain->Present(1, 0);
}

void Graphics::fillLightBuffers(Frustum&& frustum)
{
	// fill light list
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	ZeroMemory(mappedResource.pData, sizeof(LightBufferContents));

	int lightBufferCopyIndex = 0;
	for (int i = 0; i < LightList::maxSize && lightBufferCopyIndex < MAX_LIGHTS_TOTAL; ++i)
	{
		if (lightList->spotLights[i].color.w > 0.0)
		{
			CopyMemory(&lightBufferContents->lights[lightBufferCopyIndex++], &lightList->spotLights[i], sizeof(SpotLight));
		}
	}

	for (int i = 0; i < LightList::maxSize && lightBufferCopyIndex < MAX_LIGHTS_TOTAL; ++i)
	{
		if (lightList->pointLights[i].color.w > 0.0)
		{
			CopyMemory(&lightBufferContents->lights[lightBufferCopyIndex++], &lightList->pointLights[i], sizeof(PointLight));
		}
	}

	CopyMemory(mappedResource.pData, lightBufferContents, sizeof(LightBufferContents));
	deviceContext->Unmap(lightBuffer, 0);

	hr = deviceContext->Map(frustumBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &frustum, sizeof(Frustum));
	deviceContext->Unmap(frustumBuffer, 0);

	deviceContext->CSSetShader(lightCullingShader.getShader(), NULL, 0);

	ID3D11ShaderResourceView* nullSRV = NULL;
	deviceContext->PSSetShaderResources(1, 1, &nullSRV);
	deviceContext->CSSetConstantBuffers(0, 1, &lightBuffer);
	deviceContext->CSSetConstantBuffers(1, 1, &frustumBuffer);
	const UINT uavCounter = 0;
	deviceContext->CSSetUnorderedAccessViews(0, 1, &lightAppendBufferView, &uavCounter);

	deviceContext->Dispatch(MAX_LIGHTS_TOTAL, 1, 1);
	deviceContext->CopyStructureCount(lightCountBuffer, 0, lightAppendBufferView);

	// fill sun buffer
	hr = deviceContext->Map(sunBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &lightList->sun, sizeof(Sun));
	deviceContext->Unmap(sunBuffer, 0);


	ID3D11UnorderedAccessView* nullUAV = NULL;
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullUAV, 0);
	
}
