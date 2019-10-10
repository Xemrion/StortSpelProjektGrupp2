#include "ShadowMapping.h"
#include"..//game.h"


ShadowMapping::ShadowMapping()
{
}


ShadowMapping::~ShadowMapping()
{
	this->shutdown();
}

bool ShadowMapping::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;

	ID3DBlob* errorBlob = nullptr;
	HRESULT result;
	//LPCWSTR  vs = VertexShader.hlsl;
	ID3DBlob* pVS = nullptr;
	result = D3DCompileFromFile(
		L"SimpleVS.hlsl", // filename vsFilename
		nullptr,		// optional macros
		nullptr,		// optional include files
		"main",		// entry point
		"vs_5_0",		// shader model (target)
		D3DCOMPILE_DEBUG,	// shader compile options (DEBUGGING)
		0,				// IGNORE...DEPRECATED.
		&pVS,			// double pointer to ID3DBlob		
		&errorBlob		// pointer for Error Blob messages.
	);
	// compilation failed?
	if (FAILED(result))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			//OutputShaderErrorMessage(errorBlob, hwnd, vsFilename); //able when parameter active
			// release "reference" to errorBlob interface object
			errorBlob->Release();
		}
		else
		{
			//MessageBox(hwnd, vsFilename, L"Missing Shader File", MB_OK); //able when parameter active
		}
		if (pVS)
			pVS->Release();
		return false;
	}

	device->CreateVertexShader(
		pVS->GetBufferPointer(),
		pVS->GetBufferSize(),
		nullptr,
		this->simpleVertexShader.GetAddressOf()
	);

	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{
			"SV_POSITION",		// "semantic" name in shader
			0,				// "semantic" index (not used)
			DXGI_FORMAT_R32G32B32_FLOAT, // size of ONE element (3 floats)
			0,							 // input slot
			D3D11_APPEND_ALIGNED_ELEMENT, // offset of first element
			D3D11_INPUT_PER_VERTEX_DATA, // specify data PER vertex
			0							 // used for INSTANCING (ignore)
		},
		{
			"TEXCOORD",
			0,
			DXGI_FORMAT_R32G32_FLOAT, //2 values
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0
		},

		{
			"NORMAL",
			0,				// same slot as previous (same vertexBuffer)
			DXGI_FORMAT_R32G32B32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,							// offset of FIRST element (after POSITION)
			D3D11_INPUT_PER_VERTEX_DATA,
			0
			//for normal mapping. tangent binormal
		}



	};

	//int lSize = sizeof(inputDesc) / sizeof(inputDesc[0]);
	result = device->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), pVS->GetBufferPointer(), pVS->GetBufferSize(), this->vertexLayout.GetAddressOf());

	if (FAILED(result))
	{
		return false;
	}
	// we do not need anymore this COM object, so we release it.
	pVS->Release();

	D3D11_BUFFER_DESC descB = { 0 };

	
	descB.Usage = D3D11_USAGE_DYNAMIC;
	descB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	descB.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	descB.MiscFlags = 0;
	descB.ByteWidth = static_cast<UINT>(sizeof(Matrix) + (16 - (sizeof(Matrix) % 16)));
	descB.StructureByteStride = 0;

	result = device->CreateBuffer(&descB, 0, this->perFrameCB.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}
	result = device->CreateBuffer(&descB, 0, this->worldBuffer.GetAddressOf());
	if (FAILED(result))
	{
		return false;
	}


	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = 2000/*System::getWindowArea().width*/;
	depthStencilDesc.Height = 2000/*System::getWindowArea().height*/;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	this->vp.Width = (float)depthStencilDesc.Width/*System::getWindowArea().width*/;
	this->vp.Height = (float)depthStencilDesc.Height/*System::getWindowArea().height*/;
	this->vp.MinDepth = 0.0f;
	this->vp.MaxDepth = 1.0f;
	this->vp.TopLeftX = 0;
	this->vp.TopLeftY = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsv_desc;
	dsv_desc.Flags = 0;
	dsv_desc.Format = DXGI_FORMAT_D32_FLOAT;
	dsv_desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsv_desc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC sr_desc;
	sr_desc.Format = DXGI_FORMAT_R32_FLOAT;
	sr_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sr_desc.Texture2D.MostDetailedMip = 0;
	sr_desc.Texture2D.MipLevels = 1;

	if (FAILED(this->device->CreateTexture2D(&depthStencilDesc, 0, depthTexture.GetAddressOf())))
		return false;
	if (FAILED(this->device->CreateDepthStencilView(depthTexture.Get(), &dsv_desc, depthStencilView.GetAddressOf())))
		return false;
	if (FAILED(this->device->CreateShaderResourceView(depthTexture.Get(), &sr_desc, depthShaderResource.GetAddressOf())))
		return false;

	D3D11_SAMPLER_DESC desc;
	float bColor[4] = {
		1.0f, 1.0f, 1.0f, 1.0f
	};
	ZeroMemory(&desc, sizeof(desc));
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.BorderColor[0] = bColor[0];
	desc.BorderColor[1] = bColor[1];
	desc.BorderColor[2] = bColor[2];
	desc.BorderColor[3] = bColor[3];
	desc.MinLOD = 0;
	desc.MaxLOD = 1;
	HRESULT hr = this->device->CreateSamplerState(&desc, this->sampler.GetAddressOf());
	if (FAILED(hr))
	{
		//MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", "error", MB_OK);
		//deal with error. Log it maybe

	}

	return true;
}

void ShadowMapping::setWorld(const Matrix& world)
{
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &world, sizeof(Matrix));
	deviceContext->Unmap(worldBuffer.Get(), 0);
	deviceContext->VSSetConstantBuffers(1, 1, this->worldBuffer.GetAddressOf());

}

void ShadowMapping::setViewProj(DynamicCamera *camera, Vector3 sunDir)
{
	Vector3 pos(0, 5, 0);
	pos += camera->getPosition();
	view = Matrix::CreateLookAt(pos, pos + -1.0f*sunDir, Vector3(0.0f, 1.0f, 0.0f)).Transpose();
	orthoProj = Matrix::CreateOrthographic(10.0f, 10.0f, 1.0f, 7.5f);
	Matrix viewProj = view * orthoProj.Transpose();
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(perFrameCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &viewProj, sizeof(Matrix));
	deviceContext->Unmap(perFrameCB.Get(), 0);
	deviceContext->VSSetConstantBuffers(0, 1, this->perFrameCB.GetAddressOf());
}


void ShadowMapping::prepare()
{
	ID3D11ShaderResourceView* fs = NULL;
	Game::getGraphics().getDeviceContext()->PSSetShaderResources(4, 1, &fs);
	Game::getGraphics().getDeviceContext()->RSSetViewports(1, &this->vp);
	
	deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	ID3D11ShaderResourceView* s = NULL;
	deviceContext->PSSetShaderResources(3, 1, &s);
	deviceContext->OMSetRenderTargets(0, nullptr, this->depthStencilView.Get());
	deviceContext->VSSetShader(this->simpleVertexShader.Get(), nullptr, 0);
	//System::getDeviceContext()->PSSetSamplers(0, 1, &this->sampler);
	//System::getDeviceContext().omset
}

ID3D11SamplerState* ShadowMapping::getShadowSampler()
{
	return this->sampler.Get();
}


ID3D11ShaderResourceView* ShadowMapping::getShadowMap()
{
	return this->depthShaderResource.Get();
}

void ShadowMapping::shutdown()
{
	if (this->depthShaderResource != nullptr)
		this->depthShaderResource->Release();
	if (this->depthStencilView != nullptr)
		this->depthStencilView->Release();
	if (this->depthTexture != nullptr)
		this->depthTexture->Release();

}
