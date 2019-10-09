#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <d3dcompiler.h>
#include <iostream>

class VertexShader
{
public:
	bool initialize(ID3D11Device* device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC * layoutDesc, UINT numElements);
	ID3D11VertexShader * getShader();
	ID3D10Blob * getBuffer();
	ID3D11InputLayout * getInputLayout();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

class PixelShader
{
public:
	bool initialize(ID3D11Device* device, std::wstring shaderpath);
	ID3D11PixelShader * getShader();
	ID3D10Blob * getBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> shader;
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer;
};

class ShaderClass
{
public:
	//GeometryShader gs;
	PixelShader ps;
	VertexShader vs;
public:
	//bool createGS(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring fileName);
	bool createPS(ID3D11Device* device, std::wstring fileName);
	bool createVS(ID3D11Device* device, std::wstring shaderpath, D3D11_INPUT_ELEMENT_DESC* layoutDesc, UINT numElements);

};

class ComputeShader
{
public:
	bool initialize(ID3D11Device* device, std::wstring shaderPath);
	ID3D11ComputeShader* getShader();
	ID3DBlob* getBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> shader;
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBuffer;
};