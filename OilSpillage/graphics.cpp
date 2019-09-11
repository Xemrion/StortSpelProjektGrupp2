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
	if(debug!=nullptr)
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
	this->view = glm::lookAtLH(glm::vec3(0.0, 0.0, -10.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
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


	createShaders();
	debuger = new Debug(deviceContext, device);

	return true;
}

void Graphics::render()
{
	float color[4] = {
		0,0,0,1
	};
	deviceContext->ClearRenderTargetView(renderTargetView, color);
	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 1);
	deviceContext->OMSetDepthStencilState(this->depthStencilState, 0); //1

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	glm::mat4 viewProj= view * projection;
	//viewProj = glm::transpose(viewProj);
	HRESULT hr = deviceContext->Map(viewProjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, glm::value_ptr(viewProj), sizeof(glm::mat4));
	deviceContext->Unmap(viewProjBuffer, 0);


	//set up Shaders
	deviceContext->IASetInputLayout(this->shader_default.vs.GetInputLayout());
	deviceContext->PSSetShader(this->shader_default.ps.GetShader(), nullptr, 0);
	deviceContext->VSSetShader(this->shader_default.vs.GetShader(), nullptr, 0);
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
		ID3D11ShaderResourceView* shaderResView = object->getTexture()->getShaderResView();
		deviceContext->VSSetConstantBuffers(1, 1, &this->worldBuffer);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->IASetVertexBuffers(0, 1, &object->mesh->vertexBuffer, &stride, &offset);
		deviceContext->PSSetShaderResources(0, 1, &shaderResView);
		deviceContext->Draw(vertexCount, 0);
	}

	deviceContext->IASetInputLayout(this->shader_debug.vs.GetInputLayout());
	deviceContext->PSSetShader(this->shader_debug.ps.GetShader(), nullptr, 0);
	deviceContext->VSSetShader(this->shader_debug.vs.GetShader(), nullptr, 0);

	debuger->DrawLine(XMFLOAT3(0, 0, 0), XMFLOAT3(3, 2, 0 ), XMFLOAT3(1, 1, 0));
	debuger->DrawCube(XMFLOAT3(0, 0, 0), XMFLOAT3(1, 0, 0));
	//debuger->DrawRectangle(XMFLOAT3(0,0, 0), XMFLOAT3(1, 0, 0));
	
	// Present the back buffer to the screen since rendering is complete.
	swapChain->Present(0, 0);
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
		{"NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
	};


	UINT numElements = ARRAYSIZE(inputDesc);
	this->shader_default.createVS(device, shaderfolder + L"VertexShader.cso", inputDesc, numElements);
	this->shader_default.createPS(device, shaderfolder + L"PixelShader.cso");

	D3D11_INPUT_ELEMENT_DESC inputDesc2[] =
	{
		{"POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },	};
	
	UINT numElements2 = ARRAYSIZE(inputDesc2);
	this->shader_debug.createVS(device, shaderfolder + L"DebugVs.cso", inputDesc2, numElements2);
	this->shader_debug.createPS(device, shaderfolder + L"DebugPS.cso");


	//ID3DBlob* errorBlob = nullptr;
	//HRESULT result;
	////LPCWSTR  vs = VertexShader.hlsl;
	//ID3DBlob* pVS = nullptr;
	//result = D3DCompileFromFile(
	//	L"VertexShader.hlsl", // filename vsFilename
	//	nullptr,		// optional macros
	//	nullptr,		// optional include files
	//	"main",		// entry point
	//	"vs_5_0",		// shader model (target)
	//	D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
	//	0,				// IGNORE...DEPRECATED.
	//	&pVS,			// double pointer to ID3DBlob		
	//	&errorBlob		// pointer for Error Blob messages.
	//);
	//// compilation failed?
	//if (FAILED(result))
	//{
	//	if (errorBlob)
	//	{
	//		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	//		//OutputShaderErrorMessage(errorBlob, hwnd, vsFilename); //able when parameter active
	//		// release "reference" to errorBlob interface object
	//		errorBlob->Release();
	//	}
	//	else
	//	{
	//		//MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK); //able when parameter active
	//	}
	//	if (pVS)
	//		pVS->Release();
	//	return false;
	//}

	//device->CreateVertexShader(
	//	pVS->GetBufferPointer(),
	//	pVS->GetBufferSize(),
	//	nullptr,
	//	&vxShader
	//);

	//D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
	//	{
	//		"SV_POSITION",		// "semantic" name in shader
	//		0,				// "semantic" index (not used)
	//		DXGI_FORMAT_R32G32B32_FLOAT, // size of ONE element (3 floats)
	//		0,							 // input slot
	//		D3D11_APPEND_ALIGNED_ELEMENT, // offset of first element
	//		D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
	//		0							 // used for INSTANCING (ignore)
	//	},
	//	{
	//		"TEXCOORD",
	//		0,
	//		DXGI_FORMAT_R32G32_FLOAT, //2 values
	//		0,
	//		D3D11_APPEND_ALIGNED_ELEMENT,
	//		D3D11_INPUT_PER_VERTEX_DATA,
	//		0
	//	},

	//	{
	//		"NORMAL",
	//		0,				// same slot as previous (same vertexBuffer)
	//		DXGI_FORMAT_R32G32B32_FLOAT,
	//		0,
	//		D3D11_APPEND_ALIGNED_ELEMENT,							// offset of FIRST element (after POSITION)
	//		D3D11_INPUT_PER_VERTEX_DATA,
	//		0
	//		//for normal mapping. tangent binormal
	//	}
	//	   


	//};
	//
	////int lSize = sizeof(inputDesc) / sizeof(inputDesc[0]);
	//result = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), &vertexLayout);

	//if (FAILED(result))
	//{
	//	return false;
	//}
	//// we do not need anymore this COM object, so we release it.
	//pVS->Release();
	//result = D3DCompileFromFile(
	//	L"PixelShader.hlsl", // filename vsFilename
	//	nullptr,		// optional macros
	//	nullptr,		// optional include files
	//	"main",		// entry point
	//	"ps_5_0",		// shader model (target)
	//	D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
	//	0,				// IGNORE...DEPRECATED.
	//	&pVS,			// double pointer to ID3DBlob		
	//	&errorBlob		// pointer for Error Blob messages.
	//);

	//// compilation failed?
	//if (FAILED(result))
	//{
	//	if (errorBlob)
	//	{
	//		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	//		//OutputShaderErrorMessage(errorBlob, hwnd, vsFilename); //able when parameter active
	//		// release "reference" to errorBlob interface object
	//		errorBlob->Release();
	//	}
	//	else
	//	{
	//		//MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK); //able when parameter active
	//	}
	//	if (pVS)
	//		pVS->Release();
	//	return false;
	//}

	//device->CreatePixelShader(
	//	pVS->GetBufferPointer(),
	//	pVS->GetBufferSize(),
	//	nullptr,
	//	&pxShader
	//);
	
	
	





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
}

void Graphics::loadMesh(const char* fileName)
{
	Mesh newMesh;
	if (meshes.find(fileName) == meshes.end())
	{
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
		meshes[fileName].vertices.clear();//Either save vertex data or not. Depends if we want to use it for picking or something else
	}
}

void Graphics::loadShape(Shapes shape)
{
	Mesh newMesh;
	const char* fileName;
	switch (shape)
	{
	case SHAPE_CUBE:
		fileName = "Cube";
		if (meshes.find(fileName) == meshes.end())
		{

			

			std::vector<Vertex3D> vecTemp;
			meshes[fileName] = newMesh;
			Vertex3D vec[] = {
				{Vector3(-1.0,  1.0, -1.0), Vector2(0.0, 0.0), Vector3(0.0,0.0,-1.0)},
				{Vector3(1.0,1.0,-1.0), Vector2(1.0, 0.0), Vector3(0.0,0.0,-1.0)},
				{Vector3(-1.0,-1.0,-1.0), Vector2(0.0, 1.0), Vector3(0.0,0.0,-1.0)},
				{Vector3(-1.0,-1.0,-1.0), Vector2(0.0, 1.0), Vector3(0.0,0.0,-1.0)},
				{Vector3(1.0,1.0,-1.0), Vector2(1.0,0.0), Vector3(0.0,0.0,-1.0)},
				{Vector3(1.0,-1.0,-1.0), Vector2(1.0,1.0), Vector3(0.0,0.0,-1.0)},

				{Vector3(1.0,1.0,-1.0), Vector2(0.0,0.0), Vector3(1.0,0.0,0.0)},
				{Vector3(1.0,1.0,1.0), Vector2(1.0,0.0), Vector3(1.0,0.0,0.0)},
				{Vector3(1.0, -1.0, -1.0) ,Vector2(0.0, 1.0)	,Vector3(1.0,  0.0,  0.0)},
				{Vector3(1.0, -1.0, -1.0) , Vector2(0.0, 1.0)  ,Vector3(1.0,  0.0,  0.0)},
				{Vector3(1.0,  1.0,  1.0) , Vector2(1.0, 0.0)  ,Vector3(1.0,  0.0,  0.0)},
				{Vector3(1.0, -1.0,  1.0), Vector2(1.0, 1.0)	,Vector3(1.0,  0.0,  0.0)},

				{Vector3(1.0,  1.0,  1.0),Vector2(0.0, 0.0)  ,Vector3(0.0,  0.0,  1.0)},
				{Vector3(-1.0, 1.0,  1.0),Vector2(1.0, 0.0)  ,Vector3(0.0,  0.0,  1.0)},
				{Vector3(1.0, -1.0,  1.0),Vector2(0.0, 1.0)  ,Vector3(0.0,  0.0,  1.0)},
				{Vector3(1.0, -1.0,  1.0),Vector2(0.0, 1.0)  ,Vector3(0.0,  0.0,  1.0)},
				{Vector3(-1.0, 1.0,  1.0),Vector2(1.0, 0.0)  ,Vector3(0.0,  0.0,  1.0)},
				{Vector3(-1.0,-1.0,  1.0),Vector2(1.0, 1.0)  ,Vector3(0.0,  0.0,  1.0)},

				{Vector3(-1.0, 1.0,  1.0),Vector2(0.0, 0.0)	,Vector3(-1.0,  0.0,  0.0)},
				{Vector3(-1.0, 1.0, -1.0),Vector2(1.0, 0.0)	,Vector3(-1.0,  0.0,  0.0)},
				{Vector3(-1.0,-1.0,  1.0),Vector2(0.0, 1.0)	,Vector3(-1.0,  0.0,  0.0)},
				{Vector3(-1.0,-1.0,  1.0),Vector2(0.0, 1.0)	,Vector3(-1.0,  0.0,  0.0)},
				{Vector3(-1.0, 1.0, -1.0),Vector2(1.0, 0.0)	,Vector3(-1.0,  0.0,  0.0)},
				{Vector3(-1.0,-1.0, -1.0),Vector2(1.0, 1.0)	,Vector3(-1.0,  0.0,  0.0)},

				{Vector3(-1.0, 1.0,  1.0),Vector2(0.0, 0.0) ,Vector3(0.0,  1.0  ,0.0)},
				{Vector3(1.0,  1.0,  1.0),Vector2(1.0, 0.0) ,Vector3(0.0,  1.0  ,0.0)},
				{Vector3(-1.0, 1.0, -1.0),Vector2(0.0, 1.0)	,Vector3(0.0,  1.0  ,0.0)},
				{Vector3(-1.0, 1.0, -1.0),Vector2(0.0, 1.0),Vector3(0.0,  1.0  ,0.0)},
				{Vector3(1.0,  1.0,  1.0),Vector2(1.0, 0.0) ,Vector3(0.0,  1.0  ,0.0)},
				{Vector3(1.0,  1.0, -1.0),Vector2(1.0, 1.0) ,Vector3(0.0,  1.0  ,0.0)},

				{Vector3(-1.0, -1.0, -1.0),Vector2(0.0, 0.0),Vector3(0.0, -1.0 , 0.0)},
				{Vector3(1.0, -1.0,-1.0),Vector2(1.0, 0.0),Vector3(0.0, -1.0 , 0.0)},
				{Vector3(-1.0, -1.0,  1.0),Vector2(0.0, 1.0),Vector3(0.0, -1.0 , 0.0)},
				{Vector3(-1.0, -1.0,  1.0),Vector2(0.0, 1.0),Vector3(0.0, -1.0 , 0.0)},
				{Vector3(1.0, -1.0, -1.0),Vector2(1.0, 0.0),Vector3(0.0, -1.0 , 0.0)},
				{Vector3(1.0, -1.0,  1.0),Vector2(1.0, 1.0),Vector3(0.0, -1.0 , 0.0)}
			};

			for (int i = 0; i < 36; i++)
			{
				vecTemp.push_back(vec[i]);
			}


			meshes[fileName].insertDataToMesh(vecTemp);

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
			meshes[fileName].vertices.clear();//Either save vertex data or not. Depends if we want to use it for picking or something else
		}
		break;
	case SHAPE_SPHERE:
		break;
	case SHAPE_TRIANGLE:
		break;
	default:
		break;
	}
	
}

void Graphics::loadTexture(const char* fileName)
{
	Texture newTexture;
	if (textures.find(fileName) == textures.end())
	{
		textures[fileName] = newTexture;
		textures[fileName].Initialize(device, deviceContext, fileName, -1);
	}
}

const Mesh* Graphics::getMeshPointer(const char* fileName)
{
	return &meshes[fileName];
}

Texture* Graphics::getTexturePointer(const char* fileName)
{
	return &textures[fileName];
}

void Graphics::addToDraw(GameObject* o)
{
	drawableObjects.push_back(o);
}

void Graphics::removeFromDraw(GameObject* o)
{
	std::find(drawableObjects.begin(), drawableObjects.end(), o);
}
