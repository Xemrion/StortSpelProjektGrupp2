#include"ParticleSystem.h"
#include "..//game.h"
ParticleSystem::ParticleSystem()
{
	this->lastUsedParticle = 0;
	this->computeShader = nullptr;
	this->geometryShader = nullptr;
	this->pixelShader = nullptr;
	this->vertexShader = nullptr;

}

ParticleSystem::~ParticleSystem()
{
	
}

void ParticleSystem::initiateParticles(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{

	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"SV_POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"COLOR", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"CONFIG", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  },
		{"SPEED", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0  }


		
	};


	UINT numElements = ARRAYSIZE(inputDesc);


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


	std::wstring filePath = shaderfolder + L"ParticleVS.cso";

	HRESULT hr = D3DReadFileToBlob(filePath.c_str(), this->vertexShaderBlob.GetAddressOf());
	hr = device->CreateVertexShader(this->vertexShaderBlob->GetBufferPointer(), this->vertexShaderBlob->GetBufferSize(), NULL, this->vertexShader.GetAddressOf());
	hr = device->CreateInputLayout(inputDesc, numElements, this->vertexShaderBlob->GetBufferPointer(), this->vertexShaderBlob->GetBufferSize(), this->inputLayout.GetAddressOf());

	filePath = shaderfolder + L"ParticleGS.cso";
	hr = D3DReadFileToBlob(filePath.c_str(), this->geometryShaderBlob.GetAddressOf());
	hr = device->CreateGeometryShader(this->geometryShaderBlob->GetBufferPointer(), this->geometryShaderBlob->GetBufferSize(), NULL, this->geometryShader.GetAddressOf());

	filePath = shaderfolder + L"ParticlePS.cso";
	hr = D3DReadFileToBlob(filePath.c_str(), this->pixelShaderBlob.GetAddressOf());
	hr = device->CreatePixelShader(this->pixelShaderBlob->GetBufferPointer(), this->pixelShaderBlob->GetBufferSize(), NULL, this->pixelShader.GetAddressOf());

	filePath = shaderfolder + L"ParticleCreateCS.cso";
	hr = D3DReadFileToBlob(filePath.c_str(), this->createComputeShaderBlob.GetAddressOf());
	hr = device->CreateComputeShader(this->createComputeShaderBlob->GetBufferPointer(), this->createComputeShaderBlob->GetBufferSize(), NULL, this->createComputeShader.GetAddressOf());

	this->device = device;
	this->deviceContext = deviceContext;
	int bufferSize = static_cast<int>(capParticle) * sizeof(Particle);
	UINT stride = sizeof(Particle);

	D3D11_BUFFER_DESC vBufferDesc;
	ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));

	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.BindFlags = 0;
	vBufferDesc.ByteWidth = capParticle*sizeof(UINT);
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

	hr = device->CreateBuffer(&vBufferDesc, nullptr, indArgsBuffer.GetAddressOf());


	D3D11_BUFFER_DESC desc = { 0 };

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(CameraInfo) + (16 - (sizeof(CameraInfo) % 16)));
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, viewProjBuffer.GetAddressOf());

	desc.ByteWidth= static_cast<UINT>(sizeof(ParticleParams) + (16 - (sizeof(ParticleParams) % 16)));
	hr = device->CreateBuffer(&desc, 0, particleParamCB.GetAddressOf());

	desc.ByteWidth = static_cast<UINT>(sizeof(ParticleRenderParams) + (16 - (sizeof(ParticleRenderParams) % 16)));
	hr = device->CreateBuffer(&desc, 0, particleParamRenderCB.GetAddressOf());

	//Creating two buffers for the particles
	desc.ByteWidth = capParticle * sizeof(Particle);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = sizeof(Particle);
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	
	

	hr = device->CreateBuffer(&desc, 0, this->particlesBuffer.GetAddressOf());
	hr = device->CreateBuffer(&desc, 0, this->particlesBuffer2.GetAddressOf());

	//Bind the two buffers to two UAV:s   Append and consume 
	D3D11_BUFFER_UAV bUAV = { 0 };
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;

	bUAV.FirstElement = 0;
	bUAV.NumElements = capParticle;
	bUAV.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer = bUAV;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;


	hr = device->CreateUnorderedAccessView(this->particlesBuffer.Get(), &uavDesc, &this->particlesUAV);
	hr = device->CreateUnorderedAccessView(this->particlesBuffer2.Get(), &uavDesc, &this->particlesUAV2);



	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	//ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	shaderResourceViewDesc.Format = DXGI_FORMAT_UNKNOWN;
	shaderResourceViewDesc.Buffer.FirstElement = 0;
	shaderResourceViewDesc.Buffer.NumElements = capParticle;

	hr = device->CreateShaderResourceView(particlesBuffer.Get(), &shaderResourceViewDesc, particlesSRV.GetAddressOf());



}

bool ParticleSystem::addParticle(int nrOf, int lifeTime, Vector3 position, Vector3 initialDirection, Vector4 color, float size)
{
	
	UINT initialCount = 0;

	
	pParams.emitterLocation = Vector4(position.x,position.y,position.z, 1.0f);
	pParams.randomVector = Vector4(initialDirection.x, initialDirection.y, initialDirection.z, 1.0f);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(particleParamCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &pParams, sizeof(ParticleParams));
	deviceContext->Unmap(particleParamCB.Get(), 0);
	ID3D11UnorderedAccessView* n = nullptr;
	//run create particle compute shader here
	deviceContext->CSSetConstantBuffers(0, 1, this->particleParamCB.GetAddressOf());
	deviceContext->CSSetUnorderedAccessViews(0, 1, this->particlesUAV.GetAddressOf(), &initialCount);
	deviceContext->CSSetShader(this->createComputeShader.Get(), 0, 0);
	deviceContext->Dispatch(1, 1, 1);
	deviceContext->CSSetUnorderedAccessViews(0, 1, &n, &initialCount);

	return true;
}

void ParticleSystem::updateParticles(float delta)
{
	//run update computeshader here
		//deviceContext->CopyStructureCount(this->particl)
	UINT initialCount = -1;
	deviceContext->CSSetUnorderedAccessViews(0, 1, this->particlesUAV.GetAddressOf(), &initialCount);

	this->deviceContext->Dispatch(1, 1, 1);

}

void ParticleSystem::drawAll(Camera camera)
{

	CameraInfo info;
	info.viewProj= (camera.getViewMatrix() * camera.getProjectionMatrix()).Transpose();
	info.upp = Vector3(0.0, 0.0, 1.0);
	info.camPos = Vector4(camera.getPos().x, camera.getPos().y, camera.getPos().z, 0.0f);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(viewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &info, sizeof(CameraInfo));
	deviceContext->Unmap(viewProjBuffer.Get(), 0);


	ParticleRenderParams prp;
	prp.consumerLocation = Vector4(0, 0, 0, 0);
	prp.emitterLocation = pParams.emitterLocation;
	D3D11_MAPPED_SUBRESOURCE mappedResource2;
	hr = deviceContext->Map(particleParamRenderCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
	CopyMemory(mappedResource2.pData, &info, sizeof(ParticleRenderParams));
	deviceContext->Unmap(particleParamRenderCB.Get(), 0);

	UINT offset = 0;
	ID3D11ShaderResourceView* n = nullptr;

	this->deviceContext->PSSetShader(this->pixelShader.Get(), 0, 0);
	this->deviceContext->VSSetShader(this->vertexShader.Get(), 0, 0);
	this->deviceContext->GSSetShader(this->geometryShader.Get(), 0, 0);
	this->deviceContext->GSSetConstantBuffers(0, 1, this->viewProjBuffer.GetAddressOf());
	this->deviceContext->GSSetConstantBuffers(1, 1, this->particleParamRenderCB.GetAddressOf());
	this->deviceContext->VSSetShaderResources(0, 1, this->particlesSRV.GetAddressOf());
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	deviceContext->CopyStructureCount(this->indArgsBuffer.Get(), offset, this->particlesUAV.Get());
	this->deviceContext->DrawInstancedIndirect(this->indArgsBuffer.Get(), offset);
	this->deviceContext->GSSetShader(nullptr, 0, 0);
	this->deviceContext->VSSetShaderResources(0, 1, &n);

}

int ParticleSystem::findUnused()
{
	return 0;
}
