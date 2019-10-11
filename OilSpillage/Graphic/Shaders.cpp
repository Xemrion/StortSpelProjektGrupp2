#include "Shaders.h"

bool VertexShader::initialize(ID3D11Device* device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderpath;
		//Set fail messege
		return false;
	}

	hr = device->CreateVertexShader(this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr))
	{

		std::wstring errorMsg = L"Failed to create vertex shader: ";
		errorMsg += shaderpath;
		//Set fail messege
		return false;
	}

	hr = device->CreateInputLayout(layoutDesc, numElements, this->shaderBuffer->GetBufferPointer(), this->shaderBuffer->GetBufferSize(), this->inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to create input layout.";
		errorMsg += shaderpath;
		//Set fail messege
		return false;
	}

	return true;
}

ID3D11VertexShader * VertexShader::getShader()
{
	return this->shader.Get();
}

ID3D10Blob * VertexShader::getBuffer()
{
	return this->shaderBuffer.Get();
}

ID3D11InputLayout * VertexShader::getInputLayout()
{
	return this->inputLayout.Get();
}

bool PixelShader::initialize(ID3D11Device* device, std::wstring shaderpath)
{
	HRESULT hr = D3DReadFileToBlob(shaderpath.c_str(), this->shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderpath;
		//Set fail messege
		return false;
	}

	hr = device->CreatePixelShader(this->shaderBuffer.Get()->GetBufferPointer(), this->shaderBuffer.Get()->GetBufferSize(), NULL, this->shader.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderpath;
		//Set fail messege
		return false;
	}

	return true;
}

ID3D11PixelShader * PixelShader::getShader()
{
	return this->shader.Get();
}

ID3D10Blob * PixelShader::getBuffer()
{
	return this->shaderBuffer.Get();
}

bool ShaderClass::createPS(ID3D11Device* device, std::wstring fileName)
{
	return this->ps.initialize(device, fileName);
}

bool ShaderClass::createVS(ID3D11Device* device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements)
{
	return this->vs.initialize(device, shaderpath, layoutDesc, numElements);
}

bool ComputeShader::initialize(ID3D11Device* device, std::wstring shaderPath)
{
	shader.Reset();
	shaderBuffer.Reset();
	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), this->shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderPath;
		return false;
	}

	hr = device->CreateComputeShader(
		this->shaderBuffer.Get()->GetBufferPointer(),
		this->shaderBuffer.Get()->GetBufferSize(),
		NULL,
		this->shader.GetAddressOf());

	if (FAILED(hr))
	{
		std::wstring errorMsg = L"Failed to load shader: ";
		errorMsg += shaderPath;
		return false;
	}
	
	return true;
}

ID3D11ComputeShader* ComputeShader::getShader()
{
	return this->shader.Get();
}

ID3D10Blob* ComputeShader::getBuffer()
{
	return this->shaderBuffer.Get();
}

