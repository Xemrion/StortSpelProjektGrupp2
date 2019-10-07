#pragma once

#include "window.h"
#include "GameObject.h"
#include "Camera.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <unordered_map>
#include"ImGui/imgui.h"
#include"ImGui/imgui_impl_win32.h"
#include"ImGui/imgui_impl_dx11.h"
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#include "Graphic/Shaders.h"
#include "Resources/Debug.h"
#include "DynamicCamera.h"
#include<string>
#include "Lights.h"
#include <memory.h>
#include <array>

char const MODEL_ROOT_DIR[]   { "data/models/" };
char const TEXTURE_ROOT_DIR[] { "data/textures/" };

enum Shapes
{
	SHAPE_CUBE,
	SHAPE_QUAD
};

class Graphics {
	Window* window;
	IDXGISwapChain* swapChain;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	D3D11_VIEWPORT vp;

	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11RasterizerState* rasterState;
	ID3D11BlendState* alphaEnableBlendingState;
	ID3D11Buffer* viewProjBuffer;
	ID3D11Buffer* worldBuffer;
	ID3D11Buffer* colorBuffer;
	ID3D11Buffer* sunBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* frustumBuffer;
	ID3D11Buffer* culledLightBuffer;
	ID3D11UnorderedAccessView* culledLightBufferUAV;
	ID3D11ShaderResourceView* culledLightBufferSRV;
	ID3D11ShaderResourceView* frustumBufferSRV;

	ID3D11SamplerState* sampler;
	std::unordered_map<std::string, Mesh> meshes;
	std::unordered_map<std::string, Texture*> textures;
	std::vector<GameObject*> drawableObjects;
	LightList* lightList;
	
	Vector4 sunVector = Vector4(0.0, 1.0, 0.0, 0.0);
	struct LightBufferContents {
		PointLight lights[MAX_LIGHTS_TOTAL];
	};
	LightBufferContents* lightBufferContents = nullptr;

	ShaderClass shaderDefault;
	ShaderClass shaderDebug;
	ComputeShader lightCullingShader;
	Debug* debugger;
	ID3D11Debug* debug;

	void fillLightBuffers();
	void cullLights();
public:
	Graphics();
	~Graphics();
	bool init(Window* window);
	HRESULT createFrustumBuffer(DynamicCamera* camera);
	Debug* getdebugger();
	Window* getWindow();
	void loadMesh(std::string fileName);
	void loadModel(std::string fileName);
	void loadShape(Shapes shape, Vector3 normalForQuad = Vector3(0, 0, 0));
	bool loadTexture(std::string fileName);
	const Mesh* getMeshPointer(const char* fileName);
	Texture* getTexturePointer(const char* fileName);
	void addToDraw(GameObject* o);
	void removeFromDraw(GameObject* o);
	void clearDraw();
	void setLightList(LightList* lightList);
	void presentScene();
	void render(DynamicCamera* camera);
	bool createShaders();
	void clearScreen();
	ID3D11DeviceContext* getDeviceContext();
	ID3D11Device* getDevice();
};
