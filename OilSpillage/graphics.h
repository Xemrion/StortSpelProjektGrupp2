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
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	D3D11_VIEWPORT vp;

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> alphaEnableBlendingState;
	Microsoft::WRL::ComPtr<ID3D11Buffer> viewProjBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> worldBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> colorBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> sunBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> frustumBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> culledLightBuffer;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> culledLightBufferUAV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> culledLightBufferSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> frustumBufferSRV;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	std::unordered_map<std::string, Mesh> meshes;
	std::unordered_map<std::string, Texture*> textures;
	std::vector<GameObject*> drawableObjects;
	LightList* lightList;
	float cullingDistance = 150.f;
	
	Vector4 sunVector = Vector4(0.0, 1.0, 0.0, 0.0);
	struct LightBufferContents {
		PointLight lights[MAX_LIGHTS_TOTAL];
	};
	LightBufferContents* lightBufferContents = nullptr;

	ShaderClass shaderDefault;
	ShaderClass shaderDebug;
	ComputeShader lightCullingShader;
	Debug* debugger;
	Microsoft::WRL::ComPtr<ID3D11Debug> debug;

	void cullLights();
	void drawGameObjectsInstanced(std::vector<GameObject*>::iterator begin, std::vector<GameObject*>::iterator end, const Mesh* meshPtr, DynamicCamera* camera, Frustum& frustum);
public:
	Graphics();
	~Graphics();
	bool init(Window* window);
	HRESULT createFrustumBuffer(DynamicCamera* camera);
	Debug* getdebugger();
	Window* getWindow();
	void loadMesh(std::string path);
	void loadModel(std::string path);
	void loadShape(Shapes shape, Vector3 normalForQuad = Vector3(0, 0, 0));
	bool loadTexture(std::string fileName, bool overridePath=false);
	const Mesh* getMeshPointer(const char *path);
	Texture* getTexturePointer(const char *path, bool isModel=false);
	void addToDraw(GameObject* o);
	void removeFromDraw(GameObject* o);
	void clearDraw();
	void setLightList(LightList* lightList);
	void presentScene();
	void render(DynamicCamera* camera);
	bool createShaders();
	void fillLightBuffers();
	void clearScreen();
	ID3D11DeviceContext* getDeviceContext();
	ID3D11Device* getDevice();
	//culling by distance from camera
	void setCullingDistance(float dist);
	float getCullingDistance();
};
