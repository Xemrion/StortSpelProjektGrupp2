#pragma once
#include "window.h"
#include<d3d11.h>
#include<d3dcompiler.h>
#include<DirectXMath.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
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

	float fieldOfView;
	float screenNear;
	float screenDepth;
	DirectX::XMMATRIX projectionM;
	DirectX::XMMATRIX viewM;

	ID3D11Debug* debug;
public:
	Graphics();
	~Graphics();
	bool init(Window* window, float fov);
	void add();
	void render();
};
