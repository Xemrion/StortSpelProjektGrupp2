#pragma once

#include "window.h"
#include "GameObject.h"
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
#include "QuadTree.h"
#include<string>
#include "Lights.h"
#include <memory.h>
#include <array>
#include"Shadows/ShadowMapping.h"
#include"Particle/ParticleSystem.h"
#include "Structs.h"

char const MODEL_ROOT_DIR[]   { "data/models/" };
char const TEXTURE_ROOT_DIR[] { "data/textures/" };

enum Shapes
{
	SHAPE_CUBE,
	SHAPE_QUAD
};
struct MaterialColor
{
	Vector4 color;
	Vector4 shading;
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
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterStateShadow;

	Microsoft::WRL::ComPtr<ID3D11BlendState> alphaEnableBlendingState;
	Microsoft::WRL::ComPtr<ID3D11Buffer> viewProjBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> worldBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> colorBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> sunBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> lightBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> frustumBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> culledLightBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexSpot;
	Microsoft::WRL::ComPtr<ID3D11Buffer> cameraBuffer;

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
	std::unique_ptr<QuadTree> quadTree;
	
	ParticleSystem particleSystem;
	ParticleSystem particleSystem2;

	ShaderClass shaderDefault;
	ShaderClass shaderDebug;
	ComputeShader lightCullingShader;
	Debug* debugger;
	ShadowMapping shadowMap;
	Microsoft::WRL::ComPtr<ID3D11Debug> debug;

	DynamicCamera uiCamera;
	PixelShader uiPixelShader;
	VertexShader uiVertexShader;
	Sun uiSun;
	Vector3 uiSunDir = Vector3(0.0, 1.0, 0.0);
	std::vector<std::pair<GameObject*,Matrix*>> uiObjects;

	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> uiDSV;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> uiDSB;


	void cullLights();
	void drawStaticGameObjects(DynamicCamera* camera, Frustum& frustum, float frustumBias);
public:
	Graphics();
	~Graphics();
	bool init(Window* window);
	HRESULT createFrustumBuffer(DynamicCamera* camera);
	Debug* getdebugger();
	Window* getWindow();
	void loadMesh(std::string path, Vector3 rotation = Vector3(0, 0, 0));
	void loadModel(std::string path, Vector3 rotation = Vector3(0,0,0));
	void loadShape(Shapes shape, Vector3 normalForQuad = Vector3(0, 0, 0));
	bool loadTexture(std::string fileName, bool overridePath = false, bool cpuOnly = false);
	bool reloadTexture(std::string fileName, bool overridePath=false);
	const Mesh* getMeshPointer(const char *path);
	Texture* getTexturePointer(const char *path);
	Material getMaterial(const char* modelPath);
	void addToDraw(GameObject* o, bool isStatic = false);
	void removeFromDraw(GameObject* o);
	void clearDraw();
	void clearStaticObjects();

	void addToUIDraw(GameObject* obj, Matrix* world);
	void removeFromUIDraw(GameObject* obj, Matrix* world);
	void removeAllUIDraw();
	void setUISun(Vector3 direction, Vector4 color);
	void renderUI(float deltaTime);

	void setLightList(LightList* lightList);
	void presentScene();
	void render(DynamicCamera* camera, float deltaTime);
	void renderShadowmap(DynamicCamera* camera);
	bool createShaders();
	void fillLightBuffers();
	void clearScreen(Vector4 color);
	ID3D11DeviceContext* getDeviceContext();
	ID3D11Device* getDevice();
	//culling by distance from camera
	void setCullingDistance(float dist);
	float getCullingDistance();
	//Randompower means hom large the random position area is. 
	void addParticle(Vector3 pos, Vector3 initialDirection, int nrOfParticles = 2, float lifeTime = 2.0f, float randomPower = 0.5f);
	void addParticle2(Vector3 pos, Vector3 initialDirection, int nrOfParticles = 2, float lifeTime = 2.0f, float randomPower = 0.5f);
	void setParticleColorNSize(Vector4 colors[4], int nrOfColors, float startSize, float endSize);
	void setParticle2ColorNSize(Vector4 colors[4], int nrOfColors, float startSize, float endSize);
	void setVectorField(float vectorFieldSize,float vectorFieldPower);
	void setVectorField2(float vectorFieldSize,float vectorFieldPower);
	Vector3 screenToWorldSpaceUI(Vector2 screenPos);

	float farZTempShadow;
	void setSpotLighShadow(SpotLight* spotLight);
};
