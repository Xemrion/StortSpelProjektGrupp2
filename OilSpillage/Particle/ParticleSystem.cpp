#include"ParticleSystem.h"
#include "..//game.h"
#include<fstream>
ParticleSystem::ParticleSystem()
{
	this->device = nullptr;
	this->deviceContext = nullptr;
	this->indDraw.instanceCount = 1;
	//default
	this->capParticle = 51200 * 2;
	this->otherFrame = 1.0f;
	this->sinMovement = 0.0f;
	this->deltaTime = 0.0f;
	this->firstAdd = 0;
	this->nrOfParticles = 0;
	this->sP.vectorField.y = 1.0f;
	this->sP.vectorField.z = 2.0f;
	this->onlyAdd = false;
	this->bufferType = D3D11_BUFFER_UAV_FLAG::D3D11_BUFFER_UAV_FLAG_APPEND;
	this->indexForTrail = 0;
	this->texture = nullptr;
	this->quad = false;
}


ParticleSystem::~ParticleSystem()
{
	this->saveSystem();
}

void ParticleSystem::setCapacity(int cap)
{
	this->capParticle = cap;
}

void ParticleSystem::setOnlyAdd(bool arg)
{
	this->onlyAdd = arg;
}

void ParticleSystem::setNameofSystem(std::string name)
{
	strcpy(this->systemData.name, name.c_str());
}

std::string ParticleSystem::getName()
{
	return this->systemData.name;
}

void ParticleSystem::initiateParticles(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	
	
	D3D11_INPUT_ELEMENT_DESC inputDesc[] =
	{
		{"SV_InstanceID", 0, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 0  },
		{"SV_VertexID", 0, DXGI_FORMAT::DXGI_FORMAT_R32_UINT, 0,  D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 0  }
	};


	UINT numElements = ARRAYSIZE(inputDesc);


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
	


	std::wstring filePath = shaderfolder + StringToWString(this->systemData.shaders.vertexShader);;

	HRESULT hr = D3DReadFileToBlob(filePath.c_str(), this->vertexShaderBlob.GetAddressOf());
	hr = device->CreateVertexShader(this->vertexShaderBlob->GetBufferPointer(), this->vertexShaderBlob->GetBufferSize(), NULL, this->vertexShader.GetAddressOf());
	hr = device->CreateInputLayout(inputDesc, numElements, this->vertexShaderBlob->GetBufferPointer(), this->vertexShaderBlob->GetBufferSize(), this->inputLayout.GetAddressOf());

	filePath = shaderfolder + StringToWString(this->systemData.shaders.pixelShader);
	hr = D3DReadFileToBlob(filePath.c_str(), this->pixelShaderBlob.GetAddressOf());
	hr = device->CreatePixelShader(this->pixelShaderBlob->GetBufferPointer(), this->pixelShaderBlob->GetBufferSize(), NULL, this->pixelShader.GetAddressOf());

	filePath = shaderfolder + StringToWString(this->systemData.shaders.csCreate);
	hr = D3DReadFileToBlob(filePath.c_str(), this->createComputeShaderBlob.GetAddressOf());
	hr = device->CreateComputeShader(this->createComputeShaderBlob->GetBufferPointer(), this->createComputeShaderBlob->GetBufferSize(), NULL, this->createComputeShader.GetAddressOf());

	filePath = shaderfolder + StringToWString(this->systemData.shaders.csUpdate);
	hr = D3DReadFileToBlob(filePath.c_str(), this->computeShaderBlob.GetAddressOf());
	hr = device->CreateComputeShader(this->computeShaderBlob->GetBufferPointer(), this->computeShaderBlob->GetBufferSize(), NULL, this->computeShader.GetAddressOf());

	this->device = device;
	this->deviceContext = deviceContext;
	int bufferSize = static_cast<int>(capParticle) * sizeof(Particle);
	UINT stride = sizeof(Particle);

	D3D11_BUFFER_DESC vBufferDesc;
	ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));

	vBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vBufferDesc.BindFlags = 0;
	vBufferDesc.ByteWidth = sizeof(IndirDraw);
	vBufferDesc.CPUAccessFlags = 0;
	vBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
	vBufferDesc.StructureByteStride = 0;

	indDraw.instanceCount = 0;
	indDraw.vertexStartLoc = 0;
	indDraw.instanceStartLoc = 0;
	if (this->quad)
	{
		indDraw.vertexCountPerInstance = 6;//for quad
	}
	else
	{
		indDraw.vertexCountPerInstance = 3;
	}
	D3D11_SUBRESOURCE_DATA subData;
	ZeroMemory(&subData, sizeof(subData));
	subData.pSysMem = &indDraw;

	hr = device->CreateBuffer(&vBufferDesc, &subData, indArgsBuffer.GetAddressOf());

	

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
	
	desc.ByteWidth= sizeof(IndirDraw);//4 UINTs
	hr = device->CreateBuffer(&desc, 0, nrOfParticlesCB.GetAddressOf());

	desc.ByteWidth = sizeof(SimulationParams);
	hr = device->CreateBuffer(&desc, 0, simParams.GetAddressOf());

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
	bUAV.Flags = this->bufferType;

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
	hr = device->CreateShaderResourceView(particlesBuffer2.Get(), &shaderResourceViewDesc, particlesSRV2.GetAddressOf());

	D3D11_SAMPLER_DESC samplerDesc;
	float bColor[4] = {
		1.0f, 1.0f, 1.0f, 1.0f
	};
	ZeroMemory(&desc, sizeof(desc));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = bColor[0];
	samplerDesc.BorderColor[1] = bColor[1];
	samplerDesc.BorderColor[2] = bColor[2];
	samplerDesc.BorderColor[3] = bColor[3];
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 1;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 0;
	hr = device->CreateSamplerState(&samplerDesc, this->sampler.GetAddressOf());
	sP.physicsConfig.x = 0.5f;
	sP.physicsConfig.y = 1.0f;

}

bool ParticleSystem::addParticle(int nrOf, float lifeTime, Vector3 position, Vector4 initialDirection)
{
	
	if (!isfinite(position.x) || !isfinite(position.y) || !isfinite(position.z))
	{
		return false;
	}
	UINT initialCount;
	if (firstAdd == 0)
	{
		initialCount = 0;
		firstAdd = 1;
	}
	else
	{
		initialCount = -1;
	}

	pParams.initialDirection = Vector4(initialDirection.x, initialDirection.y, initialDirection.z, initialDirection.w);
	pParams.emitterLocation = Vector4(position.x,position.y,position.z, lifeTime);
	pParams.randomVector = Vector4(float(rand()) / RAND_MAX, float(rand()) / RAND_MAX, float(rand()) / RAND_MAX,1.0f);
	if(this->onlyAdd)
		pParams.randomVector.x = this->indexForTrail;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(particleParamCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &pParams, sizeof(ParticleParams));
	deviceContext->Unmap(particleParamCB.Get(), 0);
	ID3D11UnorderedAccessView* n = nullptr;
	
	ID3D11Buffer* nB = nullptr;

	this->deviceContext->CSSetConstantBuffers(2, 1, &nB);
	this->deviceContext->CSSetConstantBuffers(1, 1, &nB);
	this->deviceContext->CSSetConstantBuffers(0, 1, &nB);
	deviceContext->CSSetConstantBuffers(0, 1, this->particleParamCB.GetAddressOf());
	if (otherFrame==1 || onlyAdd)
	{
		deviceContext->CSSetUnorderedAccessViews(0, 1, this->particlesUAV.GetAddressOf(), &initialCount);
		if (onlyAdd)
		{
			deviceContext->CSSetUnorderedAccessViews(1, 1, this->particlesUAV2.GetAddressOf(), &initialCount);
		}
	}
	else
	{
		deviceContext->CSSetUnorderedAccessViews(0, 1, this->particlesUAV2.GetAddressOf(), &initialCount);
	}
	deviceContext->CSSetShader(this->createComputeShader.Get(), 0, 0);
	if (nrOf <= 0)
	{
		deviceContext->Dispatch(1, 1, 1);
	}
	else
	{
		deviceContext->Dispatch(nrOf, 1, 1);
	}
	deviceContext->CSSetUnorderedAccessViews(0, 1, &n, &initialCount);
	this->indexForTrail += 4;
	if (this->indexForTrail == this->capParticle)
	{
		this->indexForTrail = 0;
	}
	return true;
}

bool ParticleSystem::addParticle(int nrOf, float lifeTime, Vector3 position, Vector3 initialDirection)
{
	if (!isfinite(position.x) || !isfinite(position.y) || !isfinite(position.z))
	{
		return false;
	}
	nrOf = 1;
	UINT initialCount;
	if (firstAdd == 0)
	{
		initialCount = 0;
		firstAdd = 1;
	}
	else
	{
		initialCount = -1;
	}
	
	pParams.initialDirection = Vector4(initialDirection.x, initialDirection.y, initialDirection.z, 1.0f);
	pParams.emitterLocation = Vector4(position.x, position.y, position.z, lifeTime);
	pParams.randomVector = Vector4(float(rand()) / RAND_MAX, float(rand()) / RAND_MAX, float(rand()) / RAND_MAX, 1.0f);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(particleParamCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &pParams, sizeof(ParticleParams));
	deviceContext->Unmap(particleParamCB.Get(), 0);
	ID3D11UnorderedAccessView* n = nullptr;
	//run create particle compute shader here

	ID3D11Buffer* nB = nullptr;
	UINT null = 0;

	this->deviceContext->CSSetConstantBuffers(2, 1, &nB);
	this->deviceContext->CSSetConstantBuffers(1, 1, &nB);
	this->deviceContext->CSSetConstantBuffers(0, 1, &nB);
	deviceContext->CSSetConstantBuffers(0, 1, this->particleParamCB.GetAddressOf());
	if (otherFrame == 1 || onlyAdd)
	{
		deviceContext->CSSetUnorderedAccessViews(0, 1, this->particlesUAV.GetAddressOf(), &initialCount);
		if (onlyAdd)
		{
			deviceContext->CSSetUnorderedAccessViews(1, 1, this->particlesUAV2.GetAddressOf(), &initialCount);
		}
	}
	else
	{
		deviceContext->CSSetUnorderedAccessViews(0, 1, this->particlesUAV2.GetAddressOf(), &initialCount);
	}
	deviceContext->CSSetShader(this->createComputeShader.Get(), 0, 0);
	if (nrOf <= 0)
	{
		deviceContext->Dispatch(1, 1, 1);
	}
	else
	{
		deviceContext->Dispatch(nrOf, 1, 1);
	}
	deviceContext->CSSetUnorderedAccessViews(0, 1, &n, &null);
	return true;
}

void ParticleSystem::updateParticles(float delta, Matrix viewProj)
{
	ID3D11UnorderedAccessView* n = nullptr;
	UINT initialCount = -1;
	if (!onlyAdd)
	{
		ID3D11Buffer* nB = nullptr;
		ID3D11ShaderResourceView* nSRV = nullptr;
		this->deltaTime = delta;
		UINT offset = 0;
		this->sinMovement += 0.7f * delta;
		if (this->sinMovement > (26 * XM_PI) / 9)
		{
			this->sinMovement = 0.0f;
		}

		sP.vectorField.x = sinMovement;// = Vector4(sinMovement, 0.0f, 0.0f, 0.0f);
		sP.timeFactors = Vector4(delta, 0.0f, 0.0f, 0.0f);

		//run update computeshader here
		//deviceContext->CopyStructureCount(this->particl)

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(simParams.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &sP, sizeof(ParticleParams));
		deviceContext->Unmap(simParams.Get(), 0);

		
		if (otherFrame == 1.0f)
		{
			deviceContext->CopyStructureCount(this->nrOfParticlesCB.Get(), offset, this->particlesUAV.Get());
			deviceContext->CSSetUnorderedAccessViews(1, 1, this->particlesUAV.GetAddressOf(), &initialCount);
			deviceContext->CSSetUnorderedAccessViews(0, 1, this->particlesUAV2.GetAddressOf(), &initialCount);
		}
		else
		{
			deviceContext->CopyStructureCount(this->nrOfParticlesCB.Get(), offset, this->particlesUAV2.Get());
			deviceContext->CSSetUnorderedAccessViews(0, 1, this->particlesUAV.GetAddressOf(), &initialCount);
			deviceContext->CSSetUnorderedAccessViews(1, 1, this->particlesUAV2.GetAddressOf(), &initialCount);
		}
		this->deviceContext->CSSetSamplers(0, 1, this->sampler.GetAddressOf());
		this->deviceContext->CSSetConstantBuffers(1, 1, this->nrOfParticlesCB.GetAddressOf());
		this->deviceContext->CSSetConstantBuffers(0, 1, this->simParams.GetAddressOf());
		this->deviceContext->CSSetShader(this->computeShader.Get(), nullptr, 0);
		this->deviceContext->Dispatch(capParticle / 512, 1, 1);
	}
	else
	{
		ID3D11Buffer* nB = nullptr;
		ID3D11ShaderResourceView* nSRV = nullptr;
		this->deltaTime = delta;
		UINT offset = 0;
		this->sinMovement += 0.7f * delta;
		if (this->sinMovement > (26 * XM_PI) / 9)
		{
			this->sinMovement = 0.0f;
		}

		sP.vectorField.x = sinMovement;// = Vector4(sinMovement, 0.0f, 0.0f, 0.0f);
		sP.timeFactors = Vector4(delta, 0.0f, 0.0f, 0.0f);

		//run update computeshader here
		//deviceContext->CopyStructureCount(this->particl)

		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(simParams.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		CopyMemory(mappedResource.pData, &sP, sizeof(ParticleParams));
		deviceContext->Unmap(simParams.Get(), 0);

		
		deviceContext->CopyStructureCount(this->nrOfParticlesCB.Get(), offset, this->particlesUAV2.Get());
		deviceContext->CSSetUnorderedAccessViews(0, 1, this->particlesUAV2.GetAddressOf(), &initialCount);
		
		this->deviceContext->CSSetSamplers(0, 1, this->sampler.GetAddressOf());
		this->deviceContext->CSSetConstantBuffers(1, 1, this->nrOfParticlesCB.GetAddressOf());
		this->deviceContext->CSSetConstantBuffers(0, 1, this->simParams.GetAddressOf());
		this->deviceContext->CSSetShader(this->computeShader.Get(), nullptr, 0);
		this->deviceContext->Dispatch(capParticle / 512, 1, 1);
	}
	
	deviceContext->CSSetUnorderedAccessViews(0, 1, &n, &initialCount);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &n, &initialCount);
}

void ParticleSystem::changeColornSize(Vector4 colors[4], int nrOfColors, float startSize, float endSize)
{
	for (int i = 0; i < 4; i++)
	{
		this->systemData.renderParams.colors[i] = colors[i];
	}

	this->systemData.renderParams.config.x = float(nrOfColors);
	this->systemData.renderParams.config.y = startSize;
	this->systemData.renderParams.config.z = endSize;
	
}

void ParticleSystem::setSize(float startSize, float endSize)
{
	this->systemData.renderParams.config.y = startSize;
	this->systemData.renderParams.config.z = endSize;
}

void ParticleSystem::setColor(Vector4 colors[4], int nrOfColors)
{
	for (int i = 0; i < 4; i++)
	{
		this->systemData.renderParams.colors[i] = colors[i];
	}

	this->systemData.renderParams.config.x = float(nrOfColors);
}

void ParticleSystem::setMass(float mass)
{
	sP.physicsConfig.x = mass;
}

void ParticleSystem::setTexture(Texture* texture)
{
	this->texture = texture;
}

void ParticleSystem::setGravity(float gravity)
{
	sP.physicsConfig.y = gravity;
}

void ParticleSystem::changeVectorField(float vectorFieldPower, float vectorFieldSize)
{
	if (vectorFieldSize <= 0.0f)
	{
		vectorFieldSize = 1.0f;
	}
	this->systemData.vectorFieldSize = vectorFieldSize;
	this->systemData.vectorFieldPower = vectorFieldPower;
	this->sP.vectorField.y = this->systemData.vectorFieldPower;
	this->sP.vectorField.z = this->systemData.vectorFieldSize;
}
void ParticleSystem::setParticleShaders(std::string csUpdate, std::string csCreate, std::string gsPrimitive, std::string pixelShader, std::string vertexShader)
{
	strcpy(this->systemData.shaders.csUpdate, csUpdate.c_str());
	strcpy(this->systemData.shaders.csCreate, csCreate.c_str());
	strcpy(this->systemData.shaders.gsPrimitive, gsPrimitive.c_str());
	strcpy(this->systemData.shaders.pixelShader, pixelShader.c_str());
	strcpy(this->systemData.shaders.vertexShader, vertexShader.c_str());
}
void ParticleSystem::setVertexShader(std::string vertexShader)
{
	strcpy(this->systemData.shaders.vertexShader, vertexShader.c_str());
}
void ParticleSystem::setUpdateShader(std::string csUpdate)
{
	strcpy(this->systemData.shaders.csUpdate, csUpdate.c_str());
}
void ParticleSystem::setCreateShader(std::string csCreate)
{
	strcpy(this->systemData.shaders.csCreate, csCreate.c_str());
}
void ParticleSystem::setGeometryShader(std::string gsPrimitive)
{
	strcpy(this->systemData.shaders.gsPrimitive, gsPrimitive.c_str());
}
void ParticleSystem::setPixelShader(std::string pixelShader)
{
	strcpy(this->systemData.shaders.pixelShader, pixelShader.c_str());
}
void ParticleSystem::setBufferType(D3D11_BUFFER_UAV_FLAG flag)
{
	this->bufferType = flag;
}
void ParticleSystem::drawAll(DynamicCamera* camera)
{


	CameraInfo info;
	info.viewProj= (camera->getViewMatrix() * camera->getProjectionMatrix()).Transpose();
	info.upp = Vector4(0.0, 0.0, 1.0,otherFrame);
	info.camPos = Vector4(camera->getPosition().x, camera->getPosition().y, camera->getPosition().z, 0.0f);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(viewProjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &info, sizeof(CameraInfo));
	deviceContext->Unmap(viewProjBuffer.Get(), 0);


	
	D3D11_MAPPED_SUBRESOURCE mappedResource2;
	hr = deviceContext->Map(particleParamRenderCB.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
	CopyMemory(mappedResource2.pData, &this->systemData.renderParams, sizeof(ParticleRenderParams));
	deviceContext->Unmap(particleParamRenderCB.Get(), 0);

	UINT offset = 0;
	UINT copyOffset = 4;
	UINT stride = sizeof(UINT);
	ID3D11ShaderResourceView* n = nullptr;

	this->deviceContext->PSSetShader(this->pixelShader.Get(), nullptr, 0);
	this->deviceContext->VSSetShader(this->vertexShader.Get(), nullptr, 0);
	this->deviceContext->GSSetShader(nullptr, nullptr, 0);

	this->deviceContext->VSSetConstantBuffers(0, 1, this->viewProjBuffer.GetAddressOf());
	this->deviceContext->VSSetConstantBuffers(1, 1, this->particleParamRenderCB.GetAddressOf());

	ID3D11Buffer* nil = nullptr;
	
	this->deviceContext->IASetVertexBuffers(0, 1, &nil, &stride, &offset);
	this->deviceContext->IASetInputLayout(NULL);
	deviceContext->IASetInputLayout(this->inputLayout.Get());

	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	ID3D11UnorderedAccessView* nU = nullptr;

	//run create particle compute shader here
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nU, 0);
	deviceContext->CSSetUnorderedAccessViews(1, 1, &nU, 0);
	if (onlyAdd)
	{
		this->deviceContext->VSSetShaderResources(0, 1, this->particlesSRV.GetAddressOf());
		deviceContext->CopyStructureCount(this->indArgsBuffer.Get(), copyOffset, this->particlesUAV.Get());
	}
	else
	{
		if (otherFrame == 1.0f)
		{
			this->deviceContext->VSSetShaderResources(0, 1, this->particlesSRV2.GetAddressOf());
			deviceContext->CopyStructureCount(this->indArgsBuffer.Get(), copyOffset, this->particlesUAV2.Get());
		}
		else
		{
			this->deviceContext->VSSetShaderResources(0, 1, this->particlesSRV.GetAddressOf());
			deviceContext->CopyStructureCount(this->indArgsBuffer.Get(), copyOffset, this->particlesUAV.Get());
		}
	}
	//this->deviceContext->DrawInstanced(16, 1, 0, 0);
	if(this->texture!=nullptr)
	{
		ID3D11ShaderResourceView* rsv = nullptr;
		rsv = this->texture->getShaderResView();
		this->deviceContext->PSSetShaderResources(0, 1, &rsv);
	}
	this->deviceContext->DrawInstancedIndirect(this->indArgsBuffer.Get(), 0);
	if (onlyAdd)
	{
		this->deviceContext->VSSetShaderResources(0, 1, this->particlesSRV2.GetAddressOf());
		deviceContext->CopyStructureCount(this->indArgsBuffer.Get(), copyOffset, this->particlesUAV2.Get());
		this->deviceContext->DrawInstancedIndirect(this->indArgsBuffer.Get(), offset);

	}
	this->deviceContext->VSSetShaderResources(0, 1, &n);
	if (otherFrame > 0.9f)
	{
		otherFrame = 0.0f;
	}
	else
	{
		otherFrame = 1.0f;
	}
}

bool ParticleSystem::loadSystem()
{
	std::string nameTemp = systemData.name;
	std::string fileName = "data/ParticleSystems/" + nameTemp + "Particle.dat";
	std::ifstream readFile(fileName.c_str(), std::ios::out | std::ios::binary);
	if (!readFile)
	{
		return false;
	}
	readFile.read((char*)& this->systemData, sizeof(ParticleSData));
	readFile.close();
	if (!readFile.good())
	{
		//assert(false && ("Failed to read to file,, particleSystem!" + systemData.name).c_str());
		return false;
	}
	return true;
}

bool ParticleSystem::saveSystem()
{
	std::string nameTemp = systemData.name;

	std::string fileName = "data/ParticleSystems/" + nameTemp + "Particle.dat";
	std::ofstream writeFile(fileName.c_str(), std::ios::out | std::ios::binary);
	if (!writeFile)
	{
		//assert(false && ("Failed to open file, particleSystem!" + systemData.name).c_str());
		return false;
	}

	writeFile.write((char*)& this->systemData, sizeof(ParticleSData));
	writeFile.close();

	if (!writeFile.good())
	{
		//assert(false && ("Failed to write to file, particleSystem!" + systemData.name).c_str());
		return false;
	}
	return true;
}

void ParticleSystem::setQuad()
{
	this->quad = true;
}

void ParticleSystem::setShaders()
{
	this->deviceContext->PSSetShader(nullptr, nullptr, 0);

	this->deviceContext->PSSetShader(this->pixelShader.Get(), nullptr, 0);
	this->deviceContext->VSSetShader(this->vertexShader.Get(), nullptr, 0);
	this->deviceContext->GSSetShader(nullptr, nullptr, 0);
}

float ParticleSystem::getStartSize() const
{
	return this->systemData.renderParams.config.y;
}

float ParticleSystem::getEndSize() const
{
	return this->systemData.renderParams.config.z;
}

float ParticleSystem::getVectorFieldPower() const
{
	return this->systemData.vectorFieldPower;
}

float ParticleSystem::getVectorFieldSize() const
{
	return this->systemData.vectorFieldSize;
}

Vector4 ParticleSystem::getColor(int index)const
{
	return this->systemData.renderParams.colors[index];
}


