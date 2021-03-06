#include "graphics.h"
#include <algorithm>
#include"Importer/importerClass.h"
#include<cstring>
#include "ShaderDefines.hlsli"
#include "UI/UserInterface.h"
#include <cassert>
#include <atomic>

// quad tree side (+ border)
#define MAX_SIDE  68.0f

Graphics::Graphics()
{
	this->window = nullptr;

	this->vp = {};

	this->debugger = nullptr;
	/*TA BORT*/
	this->farZTempShadow = 1000.0f;
	/* */
	this->lightList = nullptr;
	this->window = nullptr;

	lightBufferContents = new LightBufferContents;

	this->particleHandler = new ParticleHandler;
	this->particleSystem = new ParticleSystem;
	this->particleSystem2 = new ParticleSystem;
	this->particleTrail = new ParticleSystem;

	this->selectedObjUI.first = nullptr;
	this->selectedObjUI.second = nullptr;

	this->particleSystem->setParticleShaders("ParticleUpdateCS.cso", "ParticleCreateCS.cso", "ParticleVS.cso");
	this->particleSystem2->setParticleShaders("ParticleUpdateCS.cso", "ParticleCreateCS.cso", "ParticleVS.cso");
	this->particleTrail->setParticleShaders("TrailUpdateCS.cso", "TrailCreateCS.cso", "TrailVS.cso", "TrailPS.cso");

	this->particleHandler->addParticleSystem(this->particleSystem,  "fire");
	this->particleHandler->addParticleSystem(this->particleSystem2, "smoke");
	//this->particleHandler->addParticleSystem(this->particleTrail,   "trail");
	this->particleTrail->setNameofSystem("trail");
	Vector4 colors[4] = {
		Vector4(0.0f,0.0f,1.0f,1.0f)
	};
	this->particleHandler->addParticleSystem("electro", colors, 1, 0.1f, 0.1f, 0.0f, 0.0f,"TrailUpdateCS.cso","ElectroCreateCS.cso","TrailVS.cso");
	//this->particleHandler->getParticleSystem("electro")->setParticleShaders("TrailUpdateCS.cso", "ElectroCreateCS.cso", "ElectroGS.cso", "ElectroPS.cso");
	Vector4 fireX[4] = {
		Vector4(1.0f,0.0f,1.0f,1.0f)
	};

	Vector4 debrisColor[4] = {
		Vector4(0.0f,0.0f,0.0f,1.0f),
		Vector4(0.0f,0.0f,0.0f,1.0f),
		Vector4(0.0f,0.0f,0.0f,1.0f),
		Vector4(0.0f,0.0f,0.0f,1.0f)
	};
	this->particleHandler->addParticleSystem("explosion","ParticleUpdateCS.cso", "ExplosionCreateCS.cso");

	this->particleHandler->addParticleSystem("debris", debrisColor, 4, 0.1f, 0.1f, 0.0f, 1.0f);

	Vector4 snowColor[4] = {
		Vector4(1.0f,1.0f,1.0f,1.0f),
		Vector4(1.0f,1.0f,1.0f,1.0f),
		Vector4(1.0f,1.0f,1.0f,1.0f),
		Vector4(0.8f,0.8f,1.0f,1.0f)
	};
	this->particleHandler->addParticleSystem("snow", snowColor, 4, 0.1f, 0.1f, 0.0f, 1.0f);
	this->particleHandler->getParticleSystem("snow")->setGravity(-0.25f);
	this->particleHandler->getParticleSystem("snow")->changeVectorField(2.5f,3.0f);


	this->particleHandler->loadParticleSystems();
	this->particleHandler->getParticleSystem("debris")->setParticleShaders("DebrisUpdateCS.cso", "DebrisCreateCS.cso", "ParticleVS.cso");
	this->particleHandler->getParticleSystem("snow")->setParticleShaders("SnowUpdateCS.cso", "SnowCreateCS.cso", "SnowParticleVS.cso");
	

	this->particleTrail->loadSystem();
	this->particleTrail->setParticleShaders("TrailUpdateCS.cso", "TrailCreateCS.cso", "TrailVS.cso", "TrailPS.cso");
	this->particleHandler->getParticleSystem("snow")->setVertexShader("SnowParticleVS.cso");
	this->particleHandler->getParticleSystem("electro")->setUpdateShader("ElectroUpdateCS.cso");
	//this->particleHandler->getParticleSystem("debris")->setParticleShaders("DebrisUpdateCS.cso","DebrisCreateCS.cso","ParticleGS.cso");
	this->quadTree = std::make_unique<QuadTree>(Vector2(-MAX_SIDE * 20.f, -MAX_SIDE * 20.f), Vector2(MAX_SIDE * 20.f, MAX_SIDE * 20.0f), 4);

	culledObjects.reserve(2000);
	culledWorldMatrices.reserve(2000);
	culledObjectsStatic.reserve(2000);
}

Graphics::~Graphics()
{
	if(debug!=nullptr)
		debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);

	delete this->debugger;
	for (auto i = textures.begin(); i != textures.end(); i++)
	{
		i->second->Shutdown();
		delete i->second;
	}
	delete this->particleTrail;
	delete this->particleHandler;
	delete this->lightBufferContents;
	swapChain->SetFullscreenState(false, NULL);
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
	swapchainDesc.BufferCount = 2;
	swapchainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapchainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapchainDesc.OutputWindow = this->window->handle;
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.Windowed = true;
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;

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
	
	D3D11_TEXTURE2D_DESC descRenderTarget;
	descRenderTarget.Width = (UINT)this->window->width;
	descRenderTarget.Height = (UINT)this->window->height;
	descRenderTarget.MipLevels = 1;
	descRenderTarget.ArraySize = 1;
	descRenderTarget.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	descRenderTarget.SampleDesc.Count = 4;
	descRenderTarget.SampleDesc.Quality = 0;
	descRenderTarget.Usage = D3D11_USAGE_DEFAULT;
	descRenderTarget.BindFlags = D3D11_BIND_RENDER_TARGET;
	descRenderTarget.CPUAccessFlags = 0;
	descRenderTarget.MiscFlags = 0;

	result = device->CreateTexture2D(&descRenderTarget, NULL, &renderTarget);
	if (FAILED(result))
	{
		return false;
	}

	device->CreateRenderTargetView(renderTarget.Get(), NULL, renderTargetView.ReleaseAndGetAddressOf());
	if (FAILED(result))
	{
		MessageBox(this->window->handle, "Could not ID3D11Texture2D* backBufferPtr", "Error", MB_OK);
		return false;
	}

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

	descDepth.Format = DXGI_FORMAT_R16_UNORM;
	descDepth.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	result = device->CreateTexture2D(&descDepth, NULL, &depthBufferCopy);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC depthSrvDesc;
	depthSrvDesc.Format = DXGI_FORMAT_R16_UNORM;
	depthSrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
	depthSrvDesc.Texture2D.MostDetailedMip = 0;
	depthSrvDesc.Texture2D.MipLevels = 1;
	
	result = device->CreateShaderResourceView(depthBufferCopy.Get(), &depthSrvDesc, &depthSRV);
	if (FAILED(result))
	{
		return false;
	}

	D3D11_RENDER_TARGET_VIEW_DESC RTVdesc;
	RTVdesc.Format = DXGI_FORMAT_R16_UNORM;
	RTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

	device->CreateRenderTargetView(depthBufferCopy.Get(), &RTVdesc, depthCopyRTV.ReleaseAndGetAddressOf());

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

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
	result = device->CreateDepthStencilState(&depthStencilDesc, readOnlyDST.GetAddressOf());
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

	desc.ByteWidth = sizeof(Matrix);
	hr = device->CreateBuffer(&desc, 0, &worldBuffer);
	if (FAILED(hr))
		return false;


	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(MaterialColor) + (16 - (sizeof(MaterialColor) % 16)));
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, &colorBuffer);
	if (FAILED(hr))
		return false;

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(Vector4));
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, &fogAnimationBuffer);
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
	desc.ByteWidth = 16;
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, &indexSpot);
	if (FAILED(hr))
		return false;

	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.ByteWidth = static_cast<UINT>(sizeof(UINT) * (MAX_LIGHTS_PER_TILE + 1) * (window->width / 16) * (window->height / 16));
	desc.StructureByteStride = static_cast<UINT>(sizeof(UINT) * (MAX_LIGHTS_PER_TILE + 1));

	hr = device->CreateBuffer(&desc, 0, &culledLightBuffer);
	if (FAILED(hr))
		return false;


	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(Vector4));
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, &cameraBuffer);
	if (FAILED(hr))
		return false;



	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = (window->width / 16) * (window->height / 16);
	uavDesc.Buffer.Flags = 0;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

	device->CreateUnorderedAccessView(culledLightBuffer.Get(), &uavDesc, &culledLightBufferUAV);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.NumElements = (window->width / 16) * (window->height / 16);

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

	D3D11_RASTERIZER_DESC rasterizerDesc2;
	ZeroMemory(&rasterizerDesc2, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc2.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc2.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;


	result = device->CreateRasterizerState(&rasterizerDesc2, &rasterStateShadow);
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

	this->device->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(debug.GetAddressOf()));

	this->shadowMap.initialize(device.Get(), deviceContext.Get());


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


	Vector4 colors[4];
	/*OIL = 0.3f, 0.2f, 0.0f, 1.0f*/
	colors[0] = Vector4(0.1f, 0.1f, 0.1f, 1.0f);
	Vector4 colorFire[4] = {
		Vector4(1.0f,1.0f,0.0f,1.0f),
		Vector4(1.0f,0.4f,0.0f,1.0f),
		Vector4(0.0f,0.0f,0.0f,1.0f),
		Vector4(0.0f,0.0f,0.0f,1.0f)
	};
	Vector4 colorsE[4] = {
		Vector4(0.0f,0.0f,1.0f,1.0f)
	};
	Vector4 fireX[4] = {
		Vector4(1.0f,1.0f,0.0f,1.0f),
		Vector4(1.0f,0.3f,0.0f,1.0f),
		Vector4(0.0f,0.0f,0.0f,1.0f),
		Vector4(0.0f,0.0f,0.0f,1.0f)
	};
	
	
	Vector4 debrisColor[4] = {
		Vector4(0.0f,0.0f,0.0f,1.0f),
		Vector4(0.0f,0.0f,0.0f,1.0f),
		Vector4(0.0f,0.0f,0.0f,1.0f),
		Vector4(0.0f,0.0f,0.0f,1.0f)
	};
	
	this->particleHandler->getParticleSystem("electro")->setVertexShader("ParticleVS.cso");
	this->particleHandler->getParticleSystem("electro")->setPixelShader("ParticlePS.cso");


	this->particleSystem->initiateParticles(device.Get(), deviceContext.Get());
	this->particleSystem2->initiateParticles(device.Get(), deviceContext.Get());
	this->particleTrail->setCapacity(512 * 20);
	this->particleTrail->setOnlyAdd(true);
	this->particleTrail->setBufferType(D3D11_BUFFER_UAV_FLAG_COUNTER);
	loadTexture("ParticleTextures/trail");
	this->particleTrail->setTexture(getTexturePointer("ParticleTextures/trail"));
	Vector4 testTC[4] = { Vector4(0.6f,0.6f,0.6f,0.1f) };
	this->particleTrail->setColor(testTC, 1);
	this->particleTrail->setQuad();
	this->particleTrail->initiateParticles(device.Get(), deviceContext.Get());
	this->particleHandler->getParticleSystem("electro")->initiateParticles(device.Get(), deviceContext.Get());
	this->particleHandler->getParticleSystem("explosion")->initiateParticles(device.Get(), deviceContext.Get());
	this->particleHandler->getParticleSystem("debris")->initiateParticles(device.Get(), deviceContext.Get());
	this->particleHandler->getParticleSystem("snow")->initiateParticles(device.Get(), deviceContext.Get());

	


	this->particleSystem->addParticle(1, 0, Vector3(0, 0, 3), Vector3(1, 0, 0));
	this->particleSystem2->addParticle(1, 0, Vector3(0, 0, 3), Vector3(1, 0, 0));
	this->particleTrail->addParticle(1, 0, Vector3(0, 0, 3), Vector3(1, 0, 0));
	this->particleHandler->getParticleSystem("electro")->addParticle(1, 0, Vector3(0, 0, 3), Vector3(1, 0, 0));
	this->particleHandler->getParticleSystem("explosion")->addParticle(1, 0, Vector3(0, 0, 3), Vector3(1, 0, 0));
	this->particleHandler->getParticleSystem("debris")->addParticle(1, 0, Vector3(0, 0, 3), Vector3(1, 0, 0));
	this->particleHandler->getParticleSystem("snow")->addParticle(1, 0, Vector3(0, 0, 3), Vector3(1, 0, 0));

	
	



	
	FogMaterial fogMaterial;
	fog = std::make_unique<Fog>();
	fogMaterial.color = Vector3(1.0, 1.0, 1.0);
	fogMaterial.scale = 5.0;
	fogMaterial.density = 0.5;
	fogMaterial.ambientDensity = 0.0;
	fogMaterial.densityThreshold = 0.5;

	uiCamera= DynamicCamera(20, 0.1f, 1000);
	uiCamera.setPosition(Vector3(0, 0, -10));

	fog->initialize(device, deviceContext, 3, 2.25, fogMaterial);
	fog->setWindSpeed(Vector2(4.0f / 1024.f, 4.0f / 1024.f));

	// Turn on the alpha blending.
	deviceContext->OMSetBlendState(alphaEnableBlendingState.Get(), blendFactor, 0xffffffff);

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

int Graphics::prepareObjects(DynamicCamera* camera)
{
	std::atomic<int> objectCount = 0;
	Frustum frustum = camera->getFrustum();
	culledObjects.resize(drawableObjects.size());
	culledWorldMatrices.resize(drawableObjects.size());

	auto cullAndPrepareWorldMatrix = [&](std::unordered_map<GameObject*, GameObject*>::iterator begin, std::unordered_map<GameObject*, GameObject*>::iterator end)
	{
		for (auto object = begin; object != end; object++)
		{
			if (Vector3::Distance(object->second->getPosition(), camera->getPosition()) < cullingDistance)
			{
				AABB boundingBox = object->second->getAABB();

				if (frustum.intersect(boundingBox, 10.0))
				{
					int index = std::atomic_fetch_add(&objectCount, 1);
					culledObjects[index] = object->second;
					SimpleMath::Matrix world = DirectX::XMMatrixTranspose(object->second->getTransform());
					culledWorldMatrices[index] = world;
				}
			}
		}
	};
	int quarterSize = drawableObjects.size() / 4;
	if (quarterSize > 0)
	{
		auto currentStart = drawableObjects.begin();
		auto currentEnd = std::next(currentStart, quarterSize);
		auto a = std::async(std::launch::async, cullAndPrepareWorldMatrix, currentStart, currentEnd);
		currentStart = currentEnd;
		currentEnd = std::next(currentStart, quarterSize);
		auto b = std::async(std::launch::async, cullAndPrepareWorldMatrix, currentStart, currentEnd);
		currentStart = currentEnd;
		currentEnd = std::next(currentStart, quarterSize);
		auto c = std::async(std::launch::async, cullAndPrepareWorldMatrix, currentStart, currentEnd);
		currentStart = currentEnd;
		currentEnd = std::next(currentStart, quarterSize);
		auto d = std::async(std::launch::async, cullAndPrepareWorldMatrix, currentStart, drawableObjects.end());

		a.wait();
		b.wait();
		c.wait();
		d.wait();
	}
	else
	{
		cullAndPrepareWorldMatrix(drawableObjects.begin(), drawableObjects.end());
	}

	return objectCount;
}

void Graphics::render(DynamicCamera* camera, float deltaTime)
{
	int culledObjectAmount = prepareObjects(camera);
	Frustum frustum = camera->getFrustum();

	float color[4] = {
		0,0,0,1
	};

	color[0] = 1.0;
	color[1] = 1.0;
	color[2] = 1.0;

	deviceContext->ClearRenderTargetView(depthCopyRTV.Get(), color);
	// Clear the depth buffer.
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	deviceContext->ClearDepthStencilView(depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);
	deviceContext->IASetInputLayout(this->shaderDefault.vs.getInputLayout());

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	renderShadowmap(camera, culledObjectAmount);
	culledObjectsStatic.clear();
	quadTreeCullingThread = std::async(std::launch::async, &QuadTree::getGameObjects, quadTree.get(), std::ref(culledObjectsStatic), frustum, 10.0f);

	deviceContext->RSSetState(rasterState.Get());

	ID3D11DepthStencilView* nulDSV = nullptr;
	ID3D11ShaderResourceView* nulSRV = nullptr;
	//deviceContext->OMSetRenderTargets(0, nullptr, nulDSV);
	ID3D11RenderTargetView* renderTargetViews[2] = { renderTargetView.Get(), depthCopyRTV.Get() };
	deviceContext->OMSetRenderTargets(2, renderTargetViews, depthStencilView.Get());

	fillLightBuffers();
	cullLights(camera->getViewMatrix());

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Matrix viewProj = (camera->getViewMatrix() * camera->getProjectionMatrix()).Transpose();
	HRESULT hr = deviceContext->Map(viewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &viewProj, sizeof(Matrix));
	deviceContext->Unmap(viewProjBuffer.Get(), 0);

	Vector4 cameraPos = Vector4(camera->getPosition());
	hr = deviceContext->Map(cameraBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &cameraPos, sizeof(Vector4));
	deviceContext->Unmap(cameraBuffer.Get(), 0);

	deviceContext->RSSetViewports(1, &this->vp);

	this->particleHandler->updateParticleSystems(deltaTime, viewProj);

	deviceContext->PSSetShaderResources(1, 1, this->shadowMap.getShadowMap().GetAddressOf());
	deviceContext->GSSetConstantBuffers(2, 1, this->shadowMap.getViewProj().GetAddressOf());
	deviceContext->PSSetSamplers(1, 1, this->shadowMap.getShadowSampler().GetAddressOf());

	this->particleHandler->renderParticleSystems(camera);

	
	//set up Shaders

	deviceContext->IASetInputLayout(this->shaderDefault.vs.getInputLayout());
	deviceContext->PSSetShader(this->shaderDefault.ps.getShader(), nullptr, 0);
	deviceContext->VSSetShader(this->shaderDefault.vs.getShader(), nullptr, 0);
	deviceContext->VSSetConstantBuffers(0, 1, this->viewProjBuffer.GetAddressOf());
	deviceContext->VSSetConstantBuffers(2, 1, this->shadowMap.getViewProj().GetAddressOf());
	deviceContext->VSSetConstantBuffers(3, 1, this->shadowMap.getViewProjSpot().GetAddressOf());

	deviceContext->PSSetSamplers(0, 1, this->sampler.GetAddressOf());
	deviceContext->PSSetShaderResources(2, 1, this->culledLightBufferSRV.GetAddressOf());
	deviceContext->PSSetShaderResources(3, 1, this->shadowMap.getShadowMap().GetAddressOf());
	deviceContext->PSSetShaderResources(4, 1, this->shadowMap.getShadowMapSpot().GetAddressOf());

	deviceContext->PSSetSamplers(1, 1, this->shadowMap.getShadowSampler().GetAddressOf());

	deviceContext->PSSetConstantBuffers(1, 1, this->lightBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(2, 1, this->sunBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(3, 1, this->indexSpot.GetAddressOf());
	deviceContext->PSSetConstantBuffers(4, 1, this->cameraBuffer.GetAddressOf());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->VSSetConstantBuffers(1, 1, this->worldBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(0, 1, this->colorBuffer.GetAddressOf());

	
	for(int objectIndex = 0; objectIndex < culledObjectAmount; objectIndex++)
	{
		GameObject* object = culledObjects[objectIndex];
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &culledWorldMatrices[objectIndex], sizeof(SimpleMath::Matrix));
		deviceContext->Unmap(worldBuffer.Get(), 0);
		UINT vertexCount = object->mesh->getVertexCount();
		UINT stride = sizeof(Vertex3D);
		UINT offset = 0;
		Material material = object->getMaterial();

		ID3D11ShaderResourceView* textureSRV = nullptr;
		if (material.diffuse != nullptr)
		{
			textureSRV = material.diffuse->getShaderResView();
		}
		ID3D11ShaderResourceView* normalSRV = nullptr;
		if (material.normal != nullptr)
		{
			normalSRV = material.normal->getShaderResView();
		}
		ID3D11ShaderResourceView* specularSRV = nullptr;
		if (material.specular != nullptr)
		{
			specularSRV = material.specular->getShaderResView();
		}
		ID3D11ShaderResourceView* glossSRV = nullptr;
		if (material.gloss != nullptr)
		{
			glossSRV = material.gloss->getShaderResView();
		}

		MaterialColor modColor;
		modColor.color = object->getColor();
		modColor.shading.x = object->getShading();
		hr = deviceContext->Map(colorBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &modColor, static_cast<UINT>(sizeof(MaterialColor) + (16 - (sizeof(MaterialColor) % 16))));
		deviceContext->Unmap(colorBuffer.Get(), 0);

		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetVertexBuffers(0, 1, object->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->PSSetShaderResources(0, 1, &textureSRV);
		deviceContext->PSSetShaderResources(1, 1, &normalSRV);
		deviceContext->PSSetShaderResources(5, 1, &specularSRV);
		deviceContext->PSSetShaderResources(6, 1, &glossSRV);
		
		deviceContext->Draw(vertexCount, 0);
	}
	quadTreeCullingThread.wait();
	drawStaticGameObjects(camera, frustum, culledObjectsStatic);
	
	this->deviceContext->PSSetShader(nullptr, nullptr, 0);

	this->particleTrail->updateParticles(deltaTime, viewProj);

	this->particleTrail->setShaders();
	deviceContext->PSSetShaderResources(1, 1, this->shadowMap.getShadowMap().GetAddressOf());
	deviceContext->PSSetSamplers(0, 1, this->sampler.GetAddressOf());
	deviceContext->VSSetConstantBuffers(2, 1, this->shadowMap.getViewProj().GetAddressOf());
	deviceContext->PSSetSamplers(1, 1, this->shadowMap.getShadowSampler().GetAddressOf());

	deviceContext->OMSetDepthStencilState(readOnlyDST.Get(), 0);


	deviceContext->PSSetConstantBuffers(1, 1, this->lightBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(2, 1, this->sunBuffer.GetAddressOf());
	deviceContext->PSSetConstantBuffers(3, 1, this->indexSpot.GetAddressOf());
	deviceContext->PSSetConstantBuffers(4, 1, this->cameraBuffer.GetAddressOf());
	deviceContext->PSSetShaderResources(2, 1, this->culledLightBufferSRV.GetAddressOf());

	this->particleTrail->drawAll(camera);
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);

	deviceContext->VSSetConstantBuffers(0, 1, this->viewProjBuffer.GetAddressOf());
	deviceContext->VSSetConstantBuffers(2, 1, this->shadowMap.getViewProj().GetAddressOf());
	deviceContext->VSSetConstantBuffers(3, 1, this->shadowMap.getViewProjSpot().GetAddressOf());
	deviceContext->IASetInputLayout(this->shaderDefault.vs.getInputLayout());

	if (fogActive)
	{
		drawFog(camera, deltaTime);
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

void Graphics::renderShadowmap(DynamicCamera* camera, int culledObjectAmount)
{
	Frustum frustum = shadowMap.getSunFrustum();
	Frustum spotFrustum = shadowMap.getSpotFrustum();
	culledObjectsStatic.clear();
	quadTreeCullingThread = std::async(std::launch::async, &QuadTree::getGameObjects, quadTree.get(), std::ref(culledObjectsStatic), frustum, 5.0f);
	//Frustum frustum = camera->getFrustum();
	shadowMap.prepare();//clear depth
	shadowMap.prepareSpot();//clear depth
	shadowMap.setViewProjSun(camera, Vector3(lightList->getSun().getDirection().x, lightList->getSun().getDirection().y, lightList->getSun().getDirection().z));
	//Setting the viewprojSpot in playinggamestate cause playerlight is there


	deviceContext->PSSetShader(nullptr, nullptr, 0);
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;

	for (int objectIndex = 0; objectIndex < culledObjectAmount; objectIndex++)
	{
		GameObject* object = culledObjects[objectIndex];
		UINT vertexCount = object->mesh->getVertexCount();
		
		shadowMap.setWorld(culledWorldMatrices[objectIndex]);
		deviceContext->IASetVertexBuffers(0, 1, object->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		
		if (object->getSunShadow())
		{
			shadowMap.setDSun();
			deviceContext->Draw(vertexCount, 0);
		}

		if (object->getSpotShadow() && spotFrustum.intersect(object->getAABB(), 10.0f, false))
		{
			shadowMap.setDSpot();
			deviceContext->Draw(vertexCount, 0);
		}
	}

	quadTreeCullingThread.wait();

	for (GameObject* o : culledObjectsStatic)
	{
		AABB boundingBox = o->getAABB();
		UINT vertexCount = o->mesh->getVertexCount();
		//SimpleMath::Matrix world = o->getTransform().Transpose();
		shadowMap.setWorld(o->getTransform().Transpose());
		deviceContext->IASetVertexBuffers(0, 1, o->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		if (o->getSunShadow())
		{
			shadowMap.setDSun();
			deviceContext->Draw(vertexCount, 0);
		}

		if (o->getSpotShadow() && spotFrustum.intersect(boundingBox, 5.0f))
		{
			shadowMap.setDSpot();
			deviceContext->Draw(vertexCount, 0);
		}
	}
	
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
		{"TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"BINORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  }
	};


	UINT numElements = ARRAYSIZE(inputDesc);
	this->shaderDefault.createVS(device.Get(), shaderfolder + L"VertexShader.cso", inputDesc, numElements);
	this->shaderDefault.createPS(device.Get(), shaderfolder + L"PixelShader.cso");
	if (!this->uiVertexShader.initialize(device.Get(), shaderfolder + L"VertexUI.cso", inputDesc, numElements))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC inputDesc2[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },	};
	
	UINT numElements2 = ARRAYSIZE(inputDesc2);
	this->shaderDebug.createVS(device.Get(), shaderfolder + L"DebugVs.cso", inputDesc2, numElements2);
	this->shaderDebug.createPS(device.Get(), shaderfolder + L"DebugPS.cso");



	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //Point
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
	this->uiPixelShader.initialize(device.Get(), shaderfolder + L"PixelUI.cso");


	return true;
}

void Graphics::addParticle(Vector3 pos, Vector3 initialDirection, int nrOfParticles,  float lifeTime, float randomPower)
{
	Vector3 randomPos = randomPower*Vector3(float(rand()), float(rand()), float(rand())) / RAND_MAX;
	Vector3 randomPos2 = -1.0f*randomPower * Vector3(float(rand()), float(rand()), float(rand())) / RAND_MAX;

	randomPos += pos;
	randomPos += randomPos2;
	float grey = float(rand()) / RAND_MAX;
	this->particleHandler->getParticleSystem("fire")->addParticleToList(lifeTime, randomPos, Vector4(initialDirection.x, initialDirection.y, initialDirection.z, 1.0f));
}

void Graphics::addParticle(std::string particleSystem, int nrOf, float lifeTime, Vector3 position, Vector4 initialDirection, float randomPower,Vector3 randomSpace)
{
	Vector3 randomPos = randomPower * Vector3(float(rand()), float(rand()), float(rand())) / RAND_MAX;
	Vector3 randomPos2 = -1.0f * randomPower * Vector3(float(rand()), float(rand()), float(rand())) / RAND_MAX;

	randomPos *= randomSpace;
	randomPos2 *= randomSpace;

	randomPos += position;
	randomPos += randomPos2;
	float grey = float(rand()) / RAND_MAX;
	this->particleHandler->getParticleSystem(particleSystem)->addParticleToList(lifeTime, randomPos, initialDirection);
}

void Graphics::addParticle2(Vector3 pos, Vector3 initialDirection, int nrOfParticles, float lifeTime, float randomPower)
{
	Vector3 randomPos = randomPower * Vector3(float(rand()), float(rand()), float(rand())) / RAND_MAX;
	Vector3 randomPos2 = -1.0f * randomPower * Vector3(float(rand()), float(rand()), float(rand())) / RAND_MAX;

	randomPos += pos;
	randomPos += randomPos2;
	float grey = float(rand()) / RAND_MAX;
	this->particleSystem2->addParticleToList(lifeTime, randomPos, Vector4(initialDirection.x, initialDirection.y, initialDirection.z, 1.0f));
}

void Graphics::setParticleColorNSize(Vector4 colors[4], int nrOfColors, float startSize, float endSize)
{
	if (nrOfColors < 5)
	{
		this->particleHandler->getParticleSystem("explosion")->changeColornSize(colors, nrOfColors, startSize, endSize);
	}
}

void Graphics::setParticle2ColorNSize(Vector4 colors[4], int nrOfColors, float startSize, float endSize)
{
	if (nrOfColors < 5)
	{
		this->particleSystem2->changeColornSize(colors, nrOfColors, startSize, endSize);
	}
}

void Graphics::setVectorField(float vectorFieldSize, float vectorFieldPower)
{
	this->particleHandler->getParticleSystem("explosion")->changeVectorField(vectorFieldPower, vectorFieldSize);
}

void Graphics::setVectorField2(float vectorFieldSize, float vectorFieldPower)
{
	this->particleSystem2->changeVectorField(vectorFieldPower, vectorFieldSize);
}

void Graphics::clearParticleSystems()
{
	this->particleHandler->clearSystems();
	this->particleTrail->clearSystem();
}

void Graphics::addTrail(Vector3 pos, Vector4 initialDirection, int nrOfParticles, float lifeTime)
{
	this->particleTrail->addParticle(nrOfParticles,lifeTime, pos, initialDirection);
}

void Graphics::changeTrailColor(Vector3 color)
{
	Vector4 colors[4];
	/*OIL = 0.3f, 0.2f, 0.0f, 1.0f*/
	colors[0] = Vector4(color.x,color.y,color.z, 1.0f);
	this->particleTrail->changeColornSize(colors, 1, 0.1f, 0.1f);
}

ParticleSystem* Graphics::getParticleSystem(std::string name)
{
	return this->particleHandler->getParticleSystem(name);
}

ParticleHandler* Graphics::getParticleHandler() const
{
	return this->particleHandler;
}

void Graphics::addTestParticle(Vector3 pos, Vector4 initialDirection, int nrOfParticles, float lifeTime, float randomPower)
{
	if (this->testParticle != nullptr)
	{
		Vector3 randomPos = randomPower * Vector3(float(rand()), float(rand()), float(rand())) / RAND_MAX;
		Vector3 randomPos2 = -1.0f * randomPower * Vector3(float(rand()), float(rand()), float(rand())) / RAND_MAX;

		randomPos += pos;
		randomPos += randomPos2;
		float grey = float(rand()) / RAND_MAX;
		this->testParticle->addParticleToList(lifeTime, randomPos, initialDirection);
	}
}

void Graphics::setTestParticleSystem(ParticleSystem* test)
{
	this->testParticle = test;
}

ParticleSystem* Graphics::getTestParticleSystem() const
{
	return this->testParticle;
}

void Graphics::saveTestParticleSystem()
{
	if (this->testParticle != nullptr)
	{
		this->testParticle->saveSystem();
	}
}

void Graphics::setTestVectorField(float vectorFieldSize, float vectorFieldPower)
{
	if (this->testParticle != nullptr)
	{
		this->testParticle->changeVectorField(vectorFieldPower, vectorFieldSize);
	}
}

void Graphics::setTestColorNSize(Vector4 colors[4], int nrOfColors, float startSize, float endSize)
{
	if (this->testParticle != nullptr)
	{
		this->testParticle->changeColornSize(colors, nrOfColors, startSize, endSize);
	}
}

Vector3 Graphics::screenToWorldSpaceUI(Vector2 screenPos)
{
	Vector2 s0((screenPos * 2.0f) / Vector2(1280, 720) - Vector2::One);
	Matrix ortho = Matrix::CreateOrthographic(1280, 720, 50.0f, 100.0f);
	//ortho = uiCamera.getProjectionMatrix().Invert();
	Vector4 w0(Vector4::Transform(Vector4(s0.x, -s0.y, 0.0f, 1.0f), uiCamera.getProjectionMatrix().Invert()/*this->uiCamera.getProjectionMatrix().Invert()*/));
	w0 *= w0.w;
	w0.w = 1.0f;
	w0 = Vector4::Transform(w0, this->uiCamera.getViewMatrix().Invert());

	return Vector3(w0.x, w0.y, 0);
}

void Graphics::clearScreen(Vector4 color)
{
	deviceContext->ClearRenderTargetView(renderTargetView.Get(), &color.x);
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

void Graphics::loadMesh( std::string const &fileName, Vector3 rotation )
{
	if ( fileName == "Cube" or fileName == "Quad" ) return;

	Mesh newMesh;

	Importer imp;
	std::string meshBinPath = fileName + "/mesh.bin";
	if (imp.loadMesh(meshBinPath.c_str()))
	{
		for (int j = 0; j < imp.getMeshCount() ; j++)
		{
			std::string meshName = fileName;
			if (j > 0)
			{
				meshName += std::to_string(j);
			}
			if (meshes.find(meshName) == meshes.end())
			{
				meshes[meshName] = newMesh;
				imp.getMeshCount();
				std::vector<Vertex3D> tempVec;
				Vertex3D vertex;
				Meshformat::Vertex* vertices = imp.getVertices(j);
				Vector3 max;
				Vector3 min;

				max.x = vertices[0].x;
				max.x = vertices[0].y;
				max.x = vertices[0].z;

				min.x = vertices[0].x;
				min.y = vertices[0].y;
				min.z = vertices[0].z;

				for (int i = 0; i < imp.getVertexCount(j); i++)
				{
					vertex.position.x = vertices[i].x;
					vertex.position.y = vertices[i].y;
					vertex.position.z = vertices[i].z;

					vertex.uv.x = vertices[i].u;
					vertex.uv.y = vertices[i].v;

					vertex.normal.x = vertices[i].nx;
					vertex.normal.y = vertices[i].ny;
					vertex.normal.z = vertices[i].nz;

					vertex.tangent.x = vertices[i].tangentX;
					vertex.tangent.y = vertices[i].tangentY;
					vertex.tangent.z = vertices[i].tangentZ;

					vertex.bitangent.x = vertices[i].binormalX;
					vertex.bitangent.y = vertices[i].binormalY;
					vertex.bitangent.z = vertices[i].binormalZ;

					int index0 = (i / 3) * 3 + 0;
					int index1 = (i / 3) * 3 + 1;
					int index2 = (i / 3) * 3 + 2;

					Vector3 v0 = Vector3(vertices[index0].x, vertices[index0].y, vertices[index0].z);
					Vector3 v1 = Vector3(vertices[index1].x, vertices[index1].y, vertices[index1].z);
					Vector3 v2 = Vector3(vertices[index2].x, vertices[index2].y, vertices[index2].z);

					Vector2 uv0 = Vector2(vertices[index0].u, vertices[index0].v);
					Vector2 uv1 = Vector2(vertices[index1].u, vertices[index1].v);
					Vector2 uv2 = Vector2(vertices[index2].u, vertices[index2].v);

					Vector3 dPos0 = v1 - v0;
					Vector3 dPos1 = v2 - v0;

					Vector2 dUV0 = uv1 - uv0;
					Vector2 dUV1 = uv2 - uv0;

					float r = 1.0f / max((dUV0.x * dUV1.y - dUV0.y * dUV1.x), 0.000001f);
					vertex.tangent = (dPos0 * dUV1.y - dPos1 * dUV0.y) * r;
					vertex.bitangent = (dPos1 * dUV0.x - dPos0 * dUV1.x) * r;
					vertex.normal.Normalize();
					vertex.tangent.Normalize();
					vertex.bitangent.Normalize();

					assert(!isnan(vertex.normal.x));
					assert(!isnan(vertex.normal.y));
					assert(!isnan(vertex.normal.z));
					assert(!isnan(vertex.tangent.x));
					assert(!isnan(vertex.tangent.y));
					assert(!isnan(vertex.tangent.z));
					assert(!isnan(vertex.bitangent.x));
					assert(!isnan(vertex.bitangent.y));
					assert(!isnan(vertex.bitangent.z));

					if (vertex.normal.Cross(vertex.tangent).Dot(vertex.bitangent) < 0.0f) {
						vertex.tangent = vertex.tangent * -1.0f;
					}

					Vector4 tempPos = Vector4::Transform(Vector4(vertex.position.x, vertex.position.y, vertex.position.z, 1.0f), Matrix::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z));
					vertex.position.x = tempPos.x;
					vertex.position.y = tempPos.y;
					vertex.position.z = tempPos.z;

					tempPos = Vector4::Transform(Vector4(vertex.normal.x, vertex.normal.y, vertex.normal.z, 0.0f), Matrix::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z));
					vertex.normal.x = tempPos.x;
					vertex.normal.y = tempPos.y;
					vertex.normal.z = tempPos.z;
					tempPos = Vector4::Transform(Vector4(vertex.tangent.x, vertex.tangent.y, vertex.tangent.z, 1.0f), Matrix::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z));
					vertex.tangent.x = tempPos.x;
					vertex.tangent.y = tempPos.y;
					vertex.tangent.z = tempPos.z;
					tempPos = Vector4::Transform(Vector4(vertex.bitangent.x, vertex.bitangent.y, vertex.bitangent.z, 1.0f), Matrix::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z));
					vertex.bitangent.x = tempPos.x;
					vertex.bitangent.y = tempPos.y;
					vertex.bitangent.z = tempPos.z;
					if (max.x < vertex.position.x)
					{
						max.x = vertex.position.x;
					}
					if (max.y < vertex.position.y)
					{
						max.y = vertex.position.y;
					}
					if (max.z < vertex.position.z)
					{
						max.z = vertex.position.z;
					}

					if (min.x > vertex.position.x)
					{
						min.x = vertex.position.x;
					}
					if (min.y > vertex.position.y)
					{
						min.y = vertex.position.y;
					}
					if (min.z > vertex.position.z)
					{
						min.z = vertex.position.z;
					}

					tempVec.push_back(vertex);
				}

				meshes[meshName].insertDataToMesh(tempVec);
				AABB aabb;
				imp.getMaxBBox(aabb.maxPos.x, aabb.maxPos.y, aabb.maxPos.z, j);
				imp.getMinBBox(aabb.minPos.x, aabb.minPos.y, aabb.minPos.z, j);

				//Calc aabb
				aabb.minPos = min;
				aabb.maxPos = max;

				meshes[meshName].setAABB(aabb);
				int bufferSize = static_cast<int>(meshes[meshName].vertices.size()) * sizeof(Vertex3D);
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
				subData.pSysMem = meshes[meshName].vertices.data();

				HRESULT hr = device->CreateBuffer(&vBufferDesc, &subData, meshes[meshName].vertexBuffer.GetAddressOf());
				meshes[fileName].vertices.clear();//Either save vertex data or not. Depends if we want to use it for picking or something else
				meshes[fileName].vertices.shrink_to_fit();
			}
		}

	}
	else assert( false and "Failed to load mesh!" );
}

void Graphics::loadMesh(std::string const& fileName, std::vector<Vertex3D>& toMesh, Vector3 rotation)
{
	Mesh newMesh;

	
	if (meshes.find(fileName) == meshes.end())
	{
		meshes[fileName] = newMesh;
		Vertex3D vertex;
		Vector3 max;
		Vector3 min;

		max.x = toMesh[0].position.x;
		max.x = toMesh[0].position.y;
		max.x = toMesh[0].position.z;

		min.x = toMesh[0].position.x;
		min.y = toMesh[0].position.y;
		min.z = toMesh[0].position.z;

		for (int i = 0; i < toMesh.size(); i++)
		{
			
			Vector4 tempPos = Vector4::Transform(Vector4(toMesh[i].position.x, toMesh[i].position.y, toMesh[i].position.z, 1.0f), Matrix::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z));
			toMesh[i].position.x = tempPos.x;
			toMesh[i].position.y = tempPos.y;
			toMesh[i].position.z = tempPos.z;

			tempPos = Vector4::Transform(Vector4(toMesh[i].normal.x, toMesh[i].normal.y, toMesh[i].normal.z, 0.0f), Matrix::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z));
			toMesh[i].normal.x = tempPos.x;
			toMesh[i].normal.y = tempPos.y;
			toMesh[i].normal.z = tempPos.z;

			tempPos = Vector4::Transform(Vector4(toMesh[i].tangent.x, toMesh[i].tangent.y, toMesh[i].tangent.z, 1.0f), Matrix::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z));
			toMesh[i].tangent.x = tempPos.x;
			toMesh[i].tangent.y = tempPos.y;
			toMesh[i].tangent.z = tempPos.z;

			tempPos = Vector4::Transform(Vector4(toMesh[i].bitangent.x, toMesh[i].bitangent.y, toMesh[i].bitangent.z, 1.0f), Matrix::CreateFromYawPitchRoll(rotation.x, rotation.y, rotation.z));
			toMesh[i].bitangent.x = tempPos.x;
			toMesh[i].bitangent.y = tempPos.y;
			toMesh[i].bitangent.z = tempPos.z;

			if (max.x < toMesh[i].position.x)
			{
				max.x = toMesh[i].position.x;
			}
			if (max.y < toMesh[i].position.y)
			{
				max.y = toMesh[i].position.y;
			}
			if (max.z < toMesh[i].position.z)
			{
				max.z = toMesh[i].position.z;
			}

			if (min.x > toMesh[i].position.x)
			{
				min.x = toMesh[i].position.x;
			}
			if (min.y > toMesh[i].position.y)
			{
				min.y = toMesh[i].position.y;
			}
			if (min.z > toMesh[i].position.z)
			{
				min.z = toMesh[i].position.z;
			}

		}

		meshes[fileName].insertDataToMesh(toMesh);
		AABB aabb;
		
		//Calc aabb
		aabb.minPos = min;
		aabb.maxPos = max;

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
		meshes[fileName].vertices.clear();//Either save vertex data or not. Depends if we want to use it for picking or something else
			
	}
	else assert(false and "Failed to load mesh!");
}

void Graphics::unloadMesh(std::string const& name)
{
	if (meshes.find(name) != meshes.end()) {
		meshes.erase(meshes.find(name));
	}
	else assert(false and "Failed to unload mesh!");
}

void Graphics::loadModel( std::string const &path, Vector3 rotation )
{
   std::string  modelDir {MODEL_ROOT_DIR};
                modelDir += path;
	loadMesh(    modelDir, rotation );
	loadMaterial( path );
}


void Graphics::loadMaterial( std::string const &path , bool tga)
{
	std::string  modelDir {MODEL_ROOT_DIR};
                modelDir += path;
	std::string fileSuffix = tga ? ".tga" : ".dds";
	loadTexture( modelDir + "/_diffuse" + fileSuffix,  true );
	loadTexture( modelDir + "/_specular" + fileSuffix, true );
	loadTexture( modelDir + "/_normal" + fileSuffix,   true );
	loadTexture( modelDir + "/_gloss" + fileSuffix,    true );
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
		}
		break;
	case SHAPE_QUAD:
		fileName = "Quad";
		if (meshes.find(fileName) == meshes.end())
		{
			std::vector<Vertex3D> vecTemp;
			Vertex3D vertex;
			meshes[fileName] = newMesh;
			vertex.position = Vector3(-1.0, -1.0, 0.0);
			vertex.uv = Vector2(0.0, 1.0);
			vertex.normal = Vector3(0.0, 0.0, -1.0);
			vertex.tangent = Vector3(1.0, 0.0, 0.0);
			vertex.bitangent = Vector3(0.0, 1.0, 0.0);
			vecTemp.push_back(vertex);

			vertex.position = Vector3(-1.0, 1.0, 0.0);
			vertex.uv = Vector2(0.0, 0.0);
			vecTemp.push_back(vertex);

			vertex.position = Vector3(1.0, -1.0, 0.0);
			vertex.uv = Vector2(1.0, 1.0);
			vecTemp.push_back(vertex);

			vertex.position = Vector3(-1.0, 1.0, 0.0);
			vertex.uv = Vector2(0.0, 0.0);
			vecTemp.push_back(vertex);

			vertex.position = Vector3(1.0, 1.0, 0.0);
			vertex.uv = Vector2(1.0, 0.0);
			vecTemp.push_back(vertex);

			vertex.position = Vector3(1.0, -1.0, 0.0);
			vertex.uv = Vector2(1.0, 1.0);
			vecTemp.push_back(vertex);

			meshes[fileName].insertDataToMesh(vecTemp);
			AABB boundingBox;
			boundingBox.minPos = Vector3(-1.0f, -1.0f, -0.01f);
			boundingBox.maxPos = Vector3(1.0f, 1.0f, 0.01f);
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

bool Graphics::loadTexture(std::string path, bool overridePath, bool cpuOnly)
{
	std::string texturePath;
	if (!overridePath) {
		texturePath += TEXTURE_ROOT_DIR;
		texturePath += path;
		texturePath += ".tga";
	}
	else texturePath = path;

	if (textures.find(texturePath) == textures.end()) {
		Texture* newTexture = new Texture();
		std::wstring wideTexturePath(texturePath.begin(), texturePath.end());
		if (!newTexture->Initialize(this->device.Get(), this->deviceContext.Get(), wideTexturePath.c_str(), -1, cpuOnly)) {
			delete newTexture;
			return false;
		}
		else 
		{
			textures[texturePath] = newTexture;
		}
	}
	return true;
}

bool Graphics::reloadTexture(std::string path, bool overridePath)
{
	std::string texturePath;
	if (!overridePath) {
		texturePath += TEXTURE_ROOT_DIR;
		texturePath += path;
		texturePath += ".tga";
	}
	else texturePath = path;

	auto texture = textures.find(texturePath);

	if (texture != textures.end()) {
		Texture* oldTexture = texture->second;
		Texture newTexture;
		std::wstring wideTexturePath(texturePath.begin(), texturePath.end());
		if (newTexture.Initialize(this->device.Get(), this->deviceContext.Get(), wideTexturePath.c_str(), -1, oldTexture->isCpuOnly()))
		{
			oldTexture->Shutdown();

			oldTexture->width = newTexture.width;
			oldTexture->height = newTexture.height;

			if (oldTexture->isCpuOnly())
			{
				oldTexture->data = newTexture.data;
				oldTexture->bpp = newTexture.bpp;
			}
			else
			{
				oldTexture->texture = newTexture.texture;
				oldTexture->textureView = newTexture.textureView;
			}
			
			return true;
		}
	}
	return false;
}

bool Graphics::unloadTexture(const char* path, bool tga)
{
	std::string texturePath;
	texturePath += TEXTURE_ROOT_DIR;
	texturePath += path;
	if (tga)
	{
		texturePath += ".tga";
	}
	else
	{
		texturePath += ".dds";
	}

	if (textures.find(texturePath) != textures.end()) {
		textures[texturePath]->Shutdown();
		delete textures[texturePath];
		textures.erase(texturePath);
		return true;
	}
	return false;
}

const Mesh* Graphics::getMeshPointer(const char* localPath)
{
   std::string meshPath;
   if (localPath != "Cube" && localPath != "Quad") 
   {
      meshPath = MODEL_ROOT_DIR;
      meshPath += localPath;
      //meshPath += "/mesh.bin";//the meshse in mehses[] wont have the mesh.bin in their name
   }
   else if (localPath != nullptr)
   {
	   meshPath = std::string(localPath);
   }

   if (meshes.find(meshPath) == meshes.end())
   {
	   return nullptr;
   }

   return &meshes[meshPath];
}

const Mesh* Graphics::getPGMeshPointer(const char* path)
{
	std::string meshPath;

	if (path != nullptr) {
		meshPath = std::string(path);
	}

	if (meshes.find(meshPath) == meshes.end())
	{
		return nullptr;
	}
	return &meshes[meshPath];
}

Texture* Graphics::getTexturePointer(const char* path, bool tga)
{
	std::string texturePath;
	texturePath += TEXTURE_ROOT_DIR;
	texturePath += path;
	if (tga)
	{
		texturePath += ".tga";
	}
	else
	{
		texturePath += ".dds";
	}

	if (textures.find(texturePath) == textures.end()) {
		//assert(false && "Failed to load texture!");
		return nullptr;
	}
	return textures[texturePath];
}

Material Graphics::getMaterial(const char* modelPath)
{
	Material material;
	std::string texturePath;
	texturePath = MODEL_ROOT_DIR + std::string(modelPath);

	if (textures.find(texturePath + "/_diffuse.tga") != textures.end())
	{
		material.diffuse = textures[texturePath + "/_diffuse.tga"];
	}
	else if (textures.find(texturePath + "/_diffuse.dds") != textures.end())
	{
		material.diffuse = textures[texturePath + "/_diffuse.dds"];
	}
	else
	{
		material.diffuse = nullptr;
	}

	if (textures.find(texturePath + "/_normal.tga") != textures.end())
	{
		material.normal = textures[texturePath + "/_normal.tga"];
	}
	else if (textures.find(texturePath + "/_normal.dds") != textures.end())
	{
		material.normal = textures[texturePath + "/_normal.dds"];
	}
	else
	{
		material.normal = nullptr;
	}

	if (textures.find(texturePath + "/_specular.tga") != textures.end())
	{
		material.specular = textures[texturePath + "/_specular.tga"];
	}
	else if (textures.find(texturePath + "/_specular.dds") != textures.end())
	{
		material.specular = textures[texturePath + "/_specular.dds"];
	}
	else
	{
		material.specular = nullptr;
	}

	if (textures.find(texturePath + "/_gloss.tga") != textures.end())
	{
		material.gloss = textures[texturePath + "/_gloss.tga"];
	}
	else if (textures.find(texturePath + "/_gloss.dds") != textures.end())
	{
		material.gloss = textures[texturePath + "/_gloss.dds"];
	}
	else
	{
		material.gloss = nullptr;
	}

	return material;
}

void Graphics::addToDrawStatic(GameObject* o)
{
	assert( o != nullptr );
	quadTree->insert(o);
}

void Graphics::addToDraw(GameObject* o)
{
	assert( o != nullptr );
	drawableObjects[o] = o;
}

void Graphics::removeFromDraw(GameObject* o)
{
	drawableObjects.erase(o);
}

void Graphics::clearDraw()
{
	drawableObjects.clear();
	quadTree->clearGameObjects();
}

void Graphics::clearStaticObjects()
{
	quadTree = std::make_unique<QuadTree>(Vector2(-MAX_SIDE * 20.f, -MAX_SIDE * 20.f), Vector2(MAX_SIDE * 20.f, MAX_SIDE * 20.0f), 4);
}

void Graphics::addToUIDraw(GameObject* obj, Matrix* world)
{
	assert(obj != nullptr);
	uiObjects.push_back(std::pair<GameObject*, Matrix*>(obj, world));
}

void Graphics::removeFromUIDraw(GameObject* obj, Matrix* world)
{
	for (auto pair = uiObjects.begin(); pair != uiObjects.end(); pair++)
	{
		if ((*pair).first == obj && (*pair).second == world)
		{
			uiObjects.erase(pair);
			break;
		}
	}
}

void Graphics::removeAllUIDraw()
{
	uiObjects.clear();
	uiObjects.shrink_to_fit();
}

void Graphics::setUISun(Vector3 direction, Vector4 color)
{
	this->uiSun.setColor(Vector3(color.x, color.y, color.z));
	this->uiSunDir = direction;
	this->uiSun.setDirection(this->uiSunDir);
}

void Graphics::renderUI(float deltaTime)
{ 
	float color[4] = {
		0,0,0,1
	};
	//deviceContext->ClearRenderTargetView(renderTargetView.Get(), color);
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);
	deviceContext->OMSetDepthStencilState(depthStencilState.Get(), 0);
	deviceContext->IASetInputLayout(this->shaderDefault.vs.getInputLayout());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext->RSSetState(rasterState.Get());


	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), this->depthStencilView.Get());
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	HRESULT hr = deviceContext->Map(sunBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &uiSun, sizeof(Sun));
	deviceContext->Unmap(sunBuffer.Get(), 0);
	
	Matrix ortho = Matrix::CreateOrthographic(1280, 720, 50.0f, 100.0f);
	ortho = uiCamera.getProjectionMatrix();
	Matrix viewProj = (uiCamera.getViewMatrix() * ortho).Transpose();
	hr = deviceContext->Map(viewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &viewProj, sizeof(Matrix));
	deviceContext->Unmap(viewProjBuffer.Get(), 0);
	

	deviceContext->RSSetViewports(1, &this->vp);

	//set up Shaders

	deviceContext->PSSetShader(this->uiPixelShader.getShader(), nullptr, 0);
	deviceContext->VSSetShader(this->uiVertexShader.getShader(), nullptr, 0);
	deviceContext->VSSetConstantBuffers(0, 1, this->viewProjBuffer.GetAddressOf());

	deviceContext->PSSetSamplers(0, 1, this->sampler.GetAddressOf());

	deviceContext->PSSetConstantBuffers(2, 1, this->sunBuffer.GetAddressOf());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	if (this->uiObjects.size()>1 && this->selectedObjUI.first != nullptr && this->selectedObjUI!=this->uiObjects[this->uiObjects.size()-1])
	{
		int posInVec = -1;
		for (int i = 0; i < this->uiObjects.size() && posInVec==-1; i++)
		{
			if (this->uiObjects[i] == this->selectedObjUI)
			{
				posInVec = i;
			}
		}
		if (posInVec != -1)
		{
			std::swap(this->uiObjects[posInVec], this->uiObjects[this->uiObjects.size() - 1]);
		}
	}
	int index = 0;
	for (std::pair<GameObject*,Matrix*> object : this->uiObjects)
	{
		if (object == this->uiObjects.back() && this->selectedObjUI.first!=nullptr)
		{
			ID3D11DepthStencilView* nulView = nullptr;

			deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);
		}
		SimpleMath::Matrix world = *object.second;//worlds[]
		SimpleMath::Matrix worldTr = DirectX::XMMatrixTranspose(world);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &worldTr, sizeof(SimpleMath::Matrix));
		deviceContext->Unmap(worldBuffer.Get(), 0);
		UINT vertexCount = object.first->mesh->getVertexCount();
		UINT stride = sizeof(Vertex3D);
		UINT offset = 0;
		Material material = object.first->getMaterial();

		ID3D11ShaderResourceView* textureSRV = nullptr;
		if (material.diffuse != nullptr)
		{
			textureSRV = material.diffuse->getShaderResView();
		}
		ID3D11ShaderResourceView* normalSRV = nullptr;
		if (material.normal != nullptr)
		{
			normalSRV = material.normal->getShaderResView();
		}
		ID3D11ShaderResourceView* specularSRV = nullptr;
		if (material.specular != nullptr)
		{
			specularSRV = material.specular->getShaderResView();
		}
		ID3D11ShaderResourceView* glossSRV = nullptr;
		if (material.gloss != nullptr)
		{
			glossSRV = material.gloss->getShaderResView();
		}

		Vector4 modColor = object.first->getColor();
		hr = deviceContext->Map(colorBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &modColor, sizeof(Vector4));
		deviceContext->Unmap(colorBuffer.Get(), 0);

		

		deviceContext->VSSetConstantBuffers(1, 1, this->worldBuffer.GetAddressOf());
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetVertexBuffers(0, 1, object.first->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->PSSetShaderResources(0, 1, &textureSRV);
		deviceContext->PSSetShaderResources(1, 1, &normalSRV);
		deviceContext->PSSetShaderResources(5, 1, &specularSRV);
		deviceContext->PSSetShaderResources(6, 1, &glossSRV);
		deviceContext->PSSetConstantBuffers(0, 1, this->colorBuffer.GetAddressOf());

		deviceContext->Draw(vertexCount, 0);
		index++;
	}

	deviceContext->IASetInputLayout(this->shaderDebug.vs.getInputLayout());
	deviceContext->PSSetShader(this->shaderDebug.ps.getShader(), nullptr, 0);
	deviceContext->VSSetShader(this->shaderDebug.vs.getShader(), nullptr, 0);
}

void Graphics::setSelectedUI(GameObject* obj, Matrix* mat)
{
	this->selectedObjUI.first = obj;
	this->selectedObjUI.second = mat;
}

void Graphics::setLightList(LightList* lightList)
{
	this->lightList = lightList;
	fillLightBuffers();
}

void Graphics::presentScene()
{
	if (window->resized)
	{
		swapChain->ResizeBuffers(2, window->width, window->height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		window->resized = false;
	}
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferPtr;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	deviceContext->ResolveSubresource(backBufferPtr.Get(), 0, renderTarget.Get(), 0, DXGI_FORMAT_R8G8B8A8_UNORM);
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

void Graphics::cullLights(Matrix view)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	view = view.Transpose();
	deviceContext->Map(viewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &view, sizeof(Matrix));
	deviceContext->Unmap(viewProjBuffer.Get(), 0);
	deviceContext->CSSetConstantBuffers(1, 1, viewProjBuffer.GetAddressOf());

	deviceContext->CSSetShader(lightCullingShader.getShader(), NULL, 0);
	ID3D11ShaderResourceView* nullSRV = NULL;
	deviceContext->PSSetShaderResources(2, 1, &nullSRV);
	deviceContext->CSSetConstantBuffers(0, 1, lightBuffer.GetAddressOf());
	deviceContext->CSSetShaderResources(0, 1, frustumBufferSRV.GetAddressOf());
	const UINT uavCounter = 0;

	deviceContext->CSSetUnorderedAccessViews(0, 1, culledLightBufferUAV.GetAddressOf(), &uavCounter);

	deviceContext->Dispatch((window->width / 16), (window->height / 16), 1);

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
	desc.ByteWidth = static_cast<UINT>(sizeof(PartialFrustum) * (window->width / 16) * (window->height / 16));
	desc.StructureByteStride = sizeof(PartialFrustum);

	PartialFrustum* frustumTiles = new PartialFrustum[(window->width / 16) * (window->height / 16)];
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
		Vector2 texCoord = Vector2(screen) / Vector2(window->width, window->height);
		texCoord.y = 1.f - texCoord.y;
		texCoord = texCoord * Vector2(2.0f, 2.0f) - Vector2(1.0f, 1.0f);
		Vector4 clip = Vector4(texCoord.x, texCoord.y, screen.z, screen.w);

		return clipToView(clip);
	};

	for (int y = 0; y < window->height / 16; ++y)
	{
		for (int x = 0; x < window->width / 16; ++x)
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

			frustumTiles[y * (window->width / 16) + x] = frustum;
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
	srvDesc.Buffer.NumElements = (window->width / 16) * (window->height / 16);

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

void Graphics::drawStaticGameObjects(DynamicCamera* camera, Frustum& frustum, std::vector<GameObject*>& objects)
{
	UINT stride = sizeof(Vertex3D);
	UINT offset = 0;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	for (GameObject* object : objects)
	{
		SimpleMath::Matrix world = object->getTransform();
		SimpleMath::Matrix worldTr = DirectX::XMMatrixTranspose(world);
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &worldTr, sizeof(SimpleMath::Matrix));
		deviceContext->Unmap(worldBuffer.Get(), 0);
		UINT vertexCount = object->mesh->getVertexCount();
		UINT stride = sizeof(Vertex3D);
		UINT offset = 0;
		Material material = object->getMaterial();

		ID3D11ShaderResourceView* textureSRV = nullptr;
		if (material.diffuse != nullptr)
		{
			textureSRV = material.diffuse->getShaderResView();
		}
		ID3D11ShaderResourceView* normalSRV = nullptr;
		if (material.normal != nullptr)
		{
			normalSRV = material.normal->getShaderResView();
		}
		ID3D11ShaderResourceView* specularSRV = nullptr;
		if (material.specular != nullptr)
		{
			specularSRV = material.specular->getShaderResView();
		}
		ID3D11ShaderResourceView* glossSRV = nullptr;
		if (material.gloss != nullptr)
		{
			glossSRV = material.gloss->getShaderResView();
		}

		MaterialColor modColor;
		modColor.color = object->getColor();
		modColor.shading.x = object->getShading();
		hr = deviceContext->Map(colorBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &modColor, static_cast<UINT>(sizeof(MaterialColor) + (16 - (sizeof(MaterialColor) % 16))));
		deviceContext->Unmap(colorBuffer.Get(), 0);


		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetVertexBuffers(0, 1, object->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->PSSetShaderResources(0, 1, &textureSRV);
		deviceContext->PSSetShaderResources(1, 1, &normalSRV);
		deviceContext->PSSetShaderResources(5, 1, &specularSRV);
		deviceContext->PSSetShaderResources(6, 1, &glossSRV);


		deviceContext->Draw(vertexCount, 0);
	}
}

void Graphics::setSpotLightShadow(SpotLight* spotLight)
{
	shadowMap.setViewProjSpot(spotLight->getPos(), spotLight->getDirection(), spotLight->getLuminance());//REMOVE LUMINANCE
	UINT index = (static_cast<Light*>(spotLight) - lightList->lights.data());
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(indexSpot.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &index, sizeof(UINT));
	deviceContext->Unmap(indexSpot.Get(), 0);
}

void Graphics::drawFog(DynamicCamera* camera, float deltaTime)
{
	time += deltaTime;
	
	deviceContext->VSSetShader(fog->drawShader.vs.getShader(), NULL, 0);
	deviceContext->PSSetShader(fog->drawShader.ps.getShader(), NULL, 0);
	
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
	deviceContext->PSSetShaderResources(7, 1, depthSRV.GetAddressOf());
	float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	deviceContext->OMSetBlendState(alphaEnableBlendingState.Get(), blendFactor, 0xffffffff);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	int i = 0;
	for (GameObject* object : fog->getQuads())
	{
		SimpleMath::Matrix world = object->getTransform();
		SimpleMath::Matrix worldTr = DirectX::XMMatrixTranspose(world);

		HRESULT hr = deviceContext->Map(worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &worldTr, sizeof(SimpleMath::Matrix));
		deviceContext->Unmap(worldBuffer.Get(), 0);
		UINT vertexCount = object->mesh->getVertexCount();
		UINT stride = sizeof(Vertex3D);
		UINT offset = 0;
		Material material = object->getMaterial();

		ID3D11ShaderResourceView* textureSRV = nullptr;
		if (material.diffuse != nullptr)
		{
			textureSRV = material.diffuse->getShaderResView();
		}
		ID3D11ShaderResourceView* normalSRV = nullptr;
		if (material.normal != nullptr)
		{
			normalSRV = material.normal->getShaderResView();
		}

		Vector4 modColor = object->getColor();
		hr = deviceContext->Map(colorBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &modColor, sizeof(Vector4));
		deviceContext->Unmap(colorBuffer.Get(), 0);

		deviceContext->VSSetConstantBuffers(1, 1, this->worldBuffer.GetAddressOf());
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetVertexBuffers(0, 1, object->mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
		deviceContext->PSSetShaderResources(0, 1, &textureSRV);
		deviceContext->PSSetShaderResources(1, 1, &normalSRV);
		deviceContext->PSSetConstantBuffers(0, 1, this->colorBuffer.GetAddressOf());

		const Vector2 windSpeed = fog->getWindSpeed();
		Vector4 t = Vector4(time, windSpeed.x + (i % 3) * 0.00001, windSpeed.y + (i % 3) * 0.000015, 0.0) * (1 / (1 + object->getPosition().y * 0.25));
		deviceContext->Map(fogAnimationBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &t, sizeof(Vector4));
		deviceContext->Unmap(fogAnimationBuffer.Get(), 0);
		deviceContext->VSSetConstantBuffers(4, 1, fogAnimationBuffer.GetAddressOf());

		deviceContext->Draw(vertexCount, 0);
		i += 1;
	}

	ID3D11ShaderResourceView* nullSRV = NULL;
	deviceContext->PSSetShaderResources(7, 1, &nullSRV);
}

void Graphics::setFog(FogMaterial material, int layers, float spacing)
{
	fog = std::make_unique<Fog>();
	float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
	deviceContext->OMSetBlendState(alphaEnableBlendingState.Get(), blendFactor, 0xffffffff);
	fog->initialize(device, deviceContext, layers, spacing, material);
	deviceContext->RSSetViewports(1, &vp);
	deviceContext->OMSetRenderTargets(1, renderTargetView.GetAddressOf(), depthStencilView.Get());
}

void Graphics::setFogWindSpeed(Vector2 speed)
{
	fog->setWindSpeed(std::move(speed));
}

void Graphics::disableFog()
{
	fogActive = false;
}

void Graphics::enableFog()
{
	fogActive = true;
}

