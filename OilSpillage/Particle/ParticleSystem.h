#pragma once
#include<d3d11.h>
#include<wrl/client.h>
#include<d3dcompiler.h>
#include"..//Camera.h"
using namespace DirectX::SimpleMath;
struct Particle
{
	Vector4 pos;
	Vector4 color;
	Vector4 config;//x = size, y = angle, z = weight, w = life
	Vector4 speed;
};
struct CameraInfo
{
	Matrix viewProj;
	Vector4 camPos;
	Vector3 upp;
};
class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();
	void initiateParticles(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	bool addParticle(int nrOf, int lifeTime, Vector3 position, Vector3 initialDirection, Vector4 color, float size);
	void updateParticles(float delta);
	void drawAll(Camera camera);
private:
	const int capParticle = 10000;
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	int nrOfParticles = 0;
	int lastUsedParticle;
	Particle* particles;
	ID3D11Buffer* particleBuffer;
	ID3D11Buffer* viewProjBuffer;


	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> pixelShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> geometryShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> computeShaderBlob;

	int findUnused();
};
