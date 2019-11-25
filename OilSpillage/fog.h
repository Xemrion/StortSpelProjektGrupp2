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

struct FogMaterial
{
	Vector3 color = Vector3(0.6, 0.6, 0.62);
	float densityThreshold = 0.15f; // remove all fog density below threshold
	float density = 0.25f; // density of fog patches
	float ambientDensity = 0.1f; // constant fog value
	float scale = 50.f; // noise scale
	float padding;
};

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
	Vector2 wind;

	ShaderClass generateTextureShader;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> textureRTV;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> normalRTV;

	void generateTextures(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, FogMaterial& material);
public:
	Fog() {};
	~Fog();
	void initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slices, float spacing, FogMaterial material);
	std::vector<GameObject*>& getQuads();
	void setWindSpeed(Vector2&& speed);
	const Vector2& getWindSpeed() const;
	ShaderClass drawShader;
};
