#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <wrl/client.h>
#include "Texture.h"
#include "GameObject.h"
#include "Graphic/Shaders.h"
#include <vector>

class Fog
{
private:
	D3D11_VIEWPORT vp;
	Texture* noiseTexture;
	Mesh* quad;
	std::vector<GameObject*> quads;
	std::vector<Texture*> fogTextures;
	std::vector<Texture*> normalTextures;
	float spacing;


	ShaderClass generateTextureShader;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> textureRTV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> normalRTV;

	void generateTextures(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext);
public:
	Fog() {};
	~Fog();
	void initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slices, float spacing);
	std::vector<GameObject*>& getQuads();
	ShaderClass drawShader;
};
