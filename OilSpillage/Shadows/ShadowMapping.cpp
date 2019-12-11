#include "ShadowMapping.h"
#include"..//game.h"


ShadowMapping::ShadowMapping()
{
	this->spotActive = false;
	this->sunActive = false;
}


ShadowMapping::~ShadowMapping()
{
}

bool ShadowMapping::initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	this->device = device;
	this->deviceContext = deviceContext;

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
	


	std::wstring filePath = shaderfolder + L"SimpleVS.cso";

	HRESULT hr = D3DReadFileToBlob(filePath.c_str(), this->vertexShaderBlob.GetAddressOf());
	hr = device->CreateVertexShader(this->vertexShaderBlob->GetBufferPointer(), this->vertexShaderBlob->GetBufferSize(), NULL, this->simpleVertexShader.GetAddressOf());

	D3D11_BUFFER_DESC descB = { 0 };

	
	descB.Usage = D3D11_USAGE_DYNAMIC;
	descB.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	descB.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	descB.MiscFlags = 0;
	descB.ByteWidth = static_cast<UINT>(sizeof(Matrix) + (16 - (sizeof(Matrix) % 16)));
	descB.StructureByteStride = 0;

	hr = device->CreateBuffer(&descB, 0, this->perFrameCB.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	hr = device->CreateBuffer(&descB, 0, this->worldBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	hr = device->CreateBuffer(&descB, 0, this->perFrameCBSpot.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = 1536;
	depthStencilDesc.Height = 1536;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	this->vp.Width = (float)depthStencilDesc.Width;
	this->vp.Height = (float)depthStencilDesc.Height;
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

	if (FAILED(this->device->CreateTexture2D(&depthStencilDesc, 0, depthTextureSpot.GetAddressOf())))
		return false;
	if (FAILED(this->device->CreateDepthStencilView(depthTextureSpot.Get(), &dsv_desc, depthStencilViewSpot.GetAddressOf())))
		return false;
	if (FAILED(this->device->CreateShaderResourceView(depthTextureSpot.Get(), &sr_desc, depthShaderResourceSpot.GetAddressOf())))
		return false;

	D3D11_SAMPLER_DESC desc;
	float bColor[4] = {
		1.0f, 1.0f, 1.0f, 1.0f
	};
	ZeroMemory(&desc, sizeof(desc));
	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	desc.BorderColor[0] = bColor[0];
	desc.BorderColor[1] = bColor[1];
	desc.BorderColor[2] = bColor[2];
	desc.BorderColor[3] = bColor[3];
	desc.MinLOD = 0;
	desc.MaxLOD = 1;
	hr = this->device->CreateSamplerState(&desc, this->sampler.GetAddressOf());
	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

void ShadowMapping::setWorld(const Matrix& world)
{
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Matrix worldTemp = world;
	HRESULT hr = deviceContext->Map(worldBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &worldTemp, sizeof(Matrix));
	deviceContext->Unmap(worldBuffer.Get(), 0);
	

}

void ShadowMapping::setViewProjSun(DynamicCamera *camera, Vector3 sunDir, float farPlaneTest)
{
	Vector3 target;
	Vector3 eye = Vector3(camera->getPosition().x, 0.0f, camera->getPosition().z);
	eye += -sunDir * 6.0f * (camera->getPosition().y / -sunDir.y);
	target = Vector3(camera->getPosition().x, 0.0f, camera->getPosition().z);
	Matrix viewMatrix;
	Matrix proj;
	Vector3 camToGroundNor = Vector3(camera->getPosition().x, 0.0f, camera->getPosition().z) - camera->getPosition();
	Vector3 camToGround = camToGroundNor;
	camToGroundNor.Normalize();
	Matrix invView = this->camera.getViewMatrix().Invert();
	Vector4 viewDir = DirectX::XMVector4Transform(Vector4(0.0, 0.0, 1.0, 0.0), invView);
	Vector4 viewUp = DirectX::XMVector4Transform(Vector4(0.0, 1.0, 0.0, 0.0), invView);
	Vector4 viewRight = DirectX::XMVector4Transform(Vector4(1.0, 0.0, 0.0, 0.0), invView);
	Vector3 farCenter = camera->getPosition() + Vector3(viewDir) * camera->getFarDistance();

	Vector3 farTopLeft = farCenter + Vector3(viewUp) * camera->getFarHeight() - Vector3(viewRight) * camera->getFarWidth();
	Vector3 farBottomLeft = farCenter - Vector3(viewUp) * camera->getFarHeight() - Vector3(viewRight) * camera->getFarWidth();
	Vector3 farTopRight = farCenter + Vector3(viewUp) * camera->getFarHeight() + Vector3(viewRight) * camera->getFarWidth();

	Vector3 dirFarTopLeft = Vector3(farTopLeft - camera->getPosition());
	dirFarTopLeft.Normalize();
	Vector3 dirFarTopRight = Vector3(farTopRight - camera->getPosition());
	dirFarTopRight.Normalize();
	Vector3 dirFarBottomLeft = Vector3(farBottomLeft - camera->getPosition());
	dirFarBottomLeft.Normalize();

	Vector3 camToLeftBotGround = camera->getPosition().y * dirFarBottomLeft;
	Vector3 camToLeftGround = camera->getPosition().y * dirFarTopLeft;
	Vector3 camToRightGround = camera->getPosition().y * dirFarTopRight;

	
	float width = (camToLeftGround - camToRightGround).Length();
	if (width <= 0.0f)
	{
		width = 90.0f;
	}
	width *= 2.0f;
	float height = (camToLeftBotGround - camToLeftGround).Length();
	if (height <= 0.0f)
	{
		height = 90.0f;
	}
	height *= 2.0f;

	viewMatrix = DirectX::XMMatrixLookAtLH(eye, target, Vector3(0, 1, 0));
	float nearDist = 10.0f;
	proj = DirectX::XMMatrixOrthographicLH(width, height, nearDist, farPlaneTest);
	Matrix viewProj = viewMatrix * proj;
	viewProj = viewProj.Transpose();
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(perFrameCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &viewProj, sizeof(Matrix));
	deviceContext->Unmap(perFrameCB.Get(), 0);

	Vector3 farPoint = eye + sunDir * farPlaneTest;
	sunFrustum.farPlane = DirectX::XMPlaneFromPointNormal(eye + sunDir * farPlaneTest, sunDir);
	sunFrustum.nearPlane = DirectX::XMPlaneFromPointNormal(eye + sunDir * nearDist, -sunDir);
	Vector3 rightVector = Vector3::Reflect(sunDir, Vector3(1.0, 0.0, 1.0));
	Vector3 upVector = Vector3::Reflect(sunDir, Vector3(0.0, 1.0, 0.0));
	
	sunFrustum.leftPlane = DirectX::XMPlaneFromPointNormal(eye - rightVector * width, -rightVector);
	sunFrustum.rightPlane = DirectX::XMPlaneFromPointNormal(eye + rightVector * width, rightVector);
	sunFrustum.bottomPlane = DirectX::XMPlaneFromPointNormal(eye - upVector * height, -upVector);
	sunFrustum.topPlane = DirectX::XMPlaneFromPointNormal(eye + upVector * height, upVector);
}

void ShadowMapping::setViewProjSpot(Vector3 pos,Vector3 dir, float fov)
{
	Vector3 tempPos = pos + Vector3(0, 0, 0);
	tempPos -= dir*1.0f;
	Vector3 target = tempPos + dir;
	Vector3 eye = tempPos;

	Matrix viewMatrix;
	Matrix proj;
	float fovTemp = 90 * (DirectX::XM_PI / 180);
	viewMatrix = DirectX::XMMatrixLookAtLH(eye, target, Vector3(0, 1, 0));
	proj = DirectX::XMMatrixPerspectiveFovLH(fovTemp, 1.0f, 1.0f, 60.0f);
	Matrix viewProj = viewMatrix * proj;
	viewProj = viewProj.Transpose();
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(perFrameCBSpot.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &viewProj, sizeof(Matrix));
	deviceContext->Unmap(perFrameCBSpot.Get(), 0);

	DynamicCamera spotCamera(pos, dir, fovTemp, 0.05f, 20.0f);
	spotFrustum = spotCamera.getFrustum();
}

void ShadowMapping::prepare()
{
	ID3D11ShaderResourceView* fs = NULL;
	Game::getGraphics().getDeviceContext()->PSSetShaderResources(3, 1, &fs);
	Game::getGraphics().getDeviceContext()->RSSetViewports(1, &this->vp);
	
	deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	ID3D11ShaderResourceView* s = NULL;
	deviceContext->PSSetShaderResources(4, 1, &s);
	deviceContext->VSSetShader(this->simpleVertexShader.Get(), nullptr, 0);
	deviceContext->VSSetConstantBuffers(1, 1, this->worldBuffer.GetAddressOf());
}

void ShadowMapping::prepareSpot()
{
	ID3D11ShaderResourceView* fs = NULL;

	deviceContext->ClearDepthStencilView(this->depthStencilViewSpot.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	ID3D11ShaderResourceView* s = NULL;
}

void ShadowMapping::setDSun()
{

		ID3D11ShaderResourceView* nSRV = nullptr;
		deviceContext->PSSetShaderResources(1, 1, &nSRV);
		deviceContext->OMSetRenderTargets(0, nullptr, this->depthStencilView.Get());
		deviceContext->VSSetConstantBuffers(0, 1, this->perFrameCB.GetAddressOf());
	


}

void ShadowMapping::setDSpot()
{
	
		ID3D11ShaderResourceView* nSRV = nullptr;
		deviceContext->PSSetShaderResources(1, 1, &nSRV);
		deviceContext->OMSetRenderTargets(0, nullptr, this->depthStencilViewSpot.Get());
		deviceContext->VSSetConstantBuffers(0, 1, this->perFrameCBSpot.GetAddressOf());
		

}

Microsoft::WRL::ComPtr <ID3D11SamplerState> ShadowMapping::getShadowSampler()
{
	return this->sampler;
}


Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> ShadowMapping::getShadowMap()
{
	return this->depthShaderResource;
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ShadowMapping::getShadowMapSpot()
{
	return this->depthShaderResourceSpot;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> ShadowMapping::getViewProj()
{
	return this->perFrameCB;
}

Microsoft::WRL::ComPtr<ID3D11Buffer> ShadowMapping::getViewProjSpot()
{
	return this->perFrameCBSpot;
}

