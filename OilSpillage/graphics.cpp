#include "graphics.h"
#include "glm/glm/ext.hpp"
#include <algorithm>

Graphics::Graphics()
{
	this->window = nullptr;
}

Graphics::~Graphics()
{
	this->depthStencilBuffer->Release();
	this->depthStencilView->Release();
	this->depthStencilState->Release();
	this->alphaEnableBlendingState->Release();
	this->rasterState->Release();
	this->renderTargetView->Release();

	if (this->swapChain)
		this->swapChain->Release();
	if (this->deviceContext)
		this->deviceContext->Release();

	debug->ReportLiveDeviceObjects(D3D11_RLDO_SUMMARY | D3D11_RLDO_DETAIL);
	if (this->device)
		this->device->Release();
	if (this->debug)
		this->debug->Release();
}

bool Graphics::init(Window* window, float fov)
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
	swapchainDesc.SampleDesc.Count = 1;
	swapchainDesc.Windowed = true;
	//swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

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
		if (FAILED(result))
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
		descDepth.SampleDesc.Count = 1;
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
			// deal with error...
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
			// deal with error...
			return false;
		}

	}

	//void SetViewport()
	this->vp.Width = (float)this->window->width;
	this->vp.Height = (float)this->window->height;
	this->vp.MinDepth = 0.0f;
	this->vp.MaxDepth = 1.0f;
	this->vp.TopLeftX = 0;
	this->vp.TopLeftY = 0;
	deviceContext->RSSetViewports(1, &this->vp);


	this->screenNear = 1;
	this->screenDepth = 1000;
	this->fieldOfView = fov * (DirectX::XM_PI / 180);

	//move to ColorShader
	this->projection = glm::perspectiveFovLH(this->fieldOfView, (float)window->width, (float)window->height, this->screenNear, this->screenDepth);
	this->view = glm::lookAtLH(glm::vec3(0.0, 10.0, 0.0), glm::vec3(0.0, 10.0, 0.0) + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	this->projection = glm::transpose(projection);
	this->view = glm::transpose(view);



	D3D11_BUFFER_DESC desc = { 0 };

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(glm::mat4) + (16 - (sizeof(glm::mat4) % 16)));
	desc.StructureByteStride = 0;

	HRESULT hr = device->CreateBuffer(&desc, 0, &viewProjBuffer);
	if (FAILED(hr))
		return false;

	hr = device->CreateBuffer(&desc, 0, &worldBuffer);
	if (FAILED(hr))
		return false;

	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;


	result = device->CreateRasterizerState(&rasterizerDesc, &rasterState);
	if (FAILED(result)) //If error occurred
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
	return true;
}

void Graphics::render()
{
	float color[4] = {
		1,0,0,1
	};
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);
	deviceContext->OMSetDepthStencilState(this->depthStencilState, 0); //1

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	glm::mat4 vp = view * projection;
	HRESULT hr = deviceContext->Map(viewProjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, glm::value_ptr(vp), sizeof(glm::mat4));
	deviceContext->Unmap(viewProjBuffer, 0);
	
	for (GameObject* object : drawableObjects)
	{
		glm::mat4 transform = glm::transpose(object->getTransform());

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(worldBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, glm::value_ptr(transform), sizeof(glm::mat4));
		deviceContext->Unmap(worldBuffer, 0);

		deviceContext->Draw(object->mesh->vertices.size(), 0);
	}

	// Present the back buffer to the screen since rendering is complete.
	swapChain->Present(0, 0);
}

void Graphics::loadMesh(const char* fileName)
{
	Mesh newMesh;
	meshes[fileName] = newMesh;
	meshes[fileName].loadMesh(fileName);

	int bufferSize = meshes[fileName].vertices.size() * sizeof(Vertex3D);
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
}

const Mesh* Graphics::getMeshPointer(const char* fileName)
{
	return &meshes[fileName];
}

void Graphics::addToDraw(GameObject* o)
{
	drawableObjects.push_back(o);
}

void Graphics::removeFromDraw(GameObject* o)
{
	std::find(drawableObjects.begin(), drawableObjects.end(), o);
}
