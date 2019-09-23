#include"ParticleSystem.h"
#include "..//game.h"
ParticleSystem::ParticleSystem()
{
	this->particles = new Particle[capParticle];
	this->lastUsedParticle = 0;
	this->computeShader = nullptr;
	this->particleBuffer = nullptr;
	this->geometryShader = nullptr;
	this->pixelShader = nullptr;
	this->vertexShader = nullptr;

}

ParticleSystem::~ParticleSystem()
{
	delete this->particles;
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

	this->device = device;
	this->deviceContext = deviceContext;
	int bufferSize = static_cast<int>(capParticle) * sizeof(Particle);
	UINT stride = sizeof(Particle);

	D3D11_BUFFER_DESC vBufferDesc;
	ZeroMemory(&vBufferDesc, sizeof(vBufferDesc));

	vBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBufferDesc.ByteWidth = bufferSize;
	vBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vBufferDesc.MiscFlags = 0;

	hr = device->CreateBuffer(&vBufferDesc, nullptr, &particleBuffer);


	D3D11_BUFFER_DESC desc = { 0 };

	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = static_cast<UINT>(sizeof(CameraInfo) + (16 - (sizeof(CameraInfo) % 16)));
	desc.StructureByteStride = 0;

	hr = device->CreateBuffer(&desc, 0, &viewProjBuffer);




}

bool ParticleSystem::addParticle(int nrOf, int lifeTime, Vector3 position, Vector3 initialDirection, Vector4 color, float size)
{
	Particle aParticle;
	aParticle.pos = Vector4(position.x,position.y,position.z,size);
	aParticle.config = Vector4(1,1,1,10);
	aParticle.color = color;
	this->particles[this->nrOfParticles] = aParticle;
	this->nrOfParticles++;
	
	aParticle.pos = Vector4(position.x+(2*size), position.y, position.z+ (2 * size), size);
	aParticle.config = Vector4(1, 1, 1, 10);
	aParticle.color = color;
	this->particles[this->nrOfParticles] = aParticle;
	this->nrOfParticles++;

	
	aParticle.pos = Vector4(position.x+(1*size), position.y+(size*1), position.z+(size*1), size);
	aParticle.config = Vector4(1, 1, 1, 10);
	aParticle.color = color;
	this->particles[this->nrOfParticles] = aParticle;
	this->nrOfParticles++;

	aParticle.pos = Vector4(position.x + 2*size, position.y+2*size, position.z + 2*size, size);
	aParticle.config = Vector4(1, 1, 1, 10);
	aParticle.color = color;
	this->particles[this->nrOfParticles] = aParticle;
	this->nrOfParticles++;


	return true;
}

void ParticleSystem::updateParticles(float delta)
{

	for (int i = 0; i < nrOfParticles; i++)
	{
		Particle& p = particles[i];
		p.speed += Vector4(0.0f, -9.81f, 0.0f,0.0f) * (float)delta * 0.1f;
		p.pos += p.speed * (float)delta;
	}
	

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(particleBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, particles, capParticle *sizeof(Particle));
	deviceContext->Unmap(particleBuffer, 0);
}

void ParticleSystem::drawAll(Camera camera)
{

	CameraInfo info;
	info.viewProj= (camera.getViewMatrix() * camera.getProjectionMatrix()).Transpose();
	info.upp = Vector3(0.0, 0.0, 1.0);
	info.camPos = Vector4(camera.getPos().x, camera.getPos().y, camera.getPos().z, 0.0f);

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = deviceContext->Map(viewProjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &info, sizeof(CameraInfo));
	deviceContext->Unmap(viewProjBuffer, 0);


	UINT stride = sizeof(Particle);
	UINT offset = 0;


	this->deviceContext->PSSetShader(this->pixelShader.Get(), 0, 0);
	this->deviceContext->VSSetShader(this->vertexShader.Get(), 0, 0);
	this->deviceContext->GSSetShader(this->geometryShader.Get(), 0, 0);
	this->deviceContext->GSSetConstantBuffers(0, 1, &this->viewProjBuffer);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	deviceContext->IASetInputLayout(this->inputLayout.Get());
	this->deviceContext->IASetVertexBuffers(0, 1, &this->particleBuffer, &stride, &offset);
	this->deviceContext->Draw(nrOfParticles, 0);
	this->deviceContext->GSSetShader(nullptr, 0, 0);

}

int ParticleSystem::findUnused()
{
	for (int i = lastUsedParticle; i < capParticle; i++)
	{
		if (particles[i].config.w < 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; i++)
	{
		if (particles[i].config.w < 0)
		{
			lastUsedParticle = i;
			return i;
		}
	}

	return 0;
}
