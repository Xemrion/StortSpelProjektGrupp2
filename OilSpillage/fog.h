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
	float densityThreshold = 0.15f; // higher value gives more holes in the fog, 1 gives only constant fog (everything is a hole)
	float density = 0.25f; // density of fog in the patches
	float ambientDensity = 0.1f; // constant minimum fog value
	float scale = 50.f; // higher value gives smaller fog patches
	float padding;
};

class Fog
{
private:
	D3D11_VIEWPORT vp;
	const int textureWidth = 1024 / 4;
	const int textureHeight = 1024 / 4;
	std::vector<GameObject*> quads;
	std::vector<Texture*> fogTextures;
	float spacing;
	Vector2 wind;

	ShaderClass generateTextureShader;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> textureRTV;


	void generateTextures(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, FogMaterial& material);
public:
	Fog() {};
	Fog(const Fog& other) = delete;
	Fog(Fog&& other) = delete;
	Fog& operator=(const Fog& other) = delete;
	~Fog();
	void initialize(Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext, UINT slices, float spacing, FogMaterial material);
	std::vector<GameObject*>& getQuads();
	void setWindSpeed(Vector2&& speed);
	const Vector2& getWindSpeed() const;
	ShaderClass drawShader;
};
