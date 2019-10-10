#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include<SimpleMath.h>
#include<wrl/client.h>
#include"..///DynamicCamera.h"
class ShadowMapping
{
public:
	ShadowMapping();
	virtual~ShadowMapping();

	bool initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	void setWorld(const Matrix& world);
	void setViewProj(DynamicCamera* camera, Vector3 sunDir);
	//uses the rendershader in Shader
	void prepare();
	ID3D11SamplerState* getShadowSampler();
	ID3D11ShaderResourceView* getShadowMap();
private:

	Microsoft::WRL::ComPtr<ID3D11BlendState> blendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasState;
	
	D3D11_VIEWPORT vp;
	Microsoft::WRL::ComPtr <ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr <ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr <ID3D11Texture2D> depthTexture;
	Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> depthShaderResource;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> simpleVertexShader;//only for creating the shadowmap
	Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> vertexLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> worldBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> perFrameCB;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	DynamicCamera camera;
	Matrix view;
	Matrix orthoProj;


};


