#pragma once
#include<d3d11.h>
#include<wrl/client.h>
#include<d3dcompiler.h>
#include"..//Camera.h"
using namespace DirectX::SimpleMath;
struct Particle
{
	Vector3 position;
	Vector3 direction;
	float time;
};
struct CameraInfo
{
	Matrix viewProj;
	Vector4 camPos;
	Vector3 upp;
};
struct ParticleParams
{
	Vector4 emitterLocation;
	Vector4 randomVector;
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
	//Particle* particles;
	//ID3D11Buffer* particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> particleParamCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> viewProjBuffer;


	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> pixelShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11GeometryShader> geometryShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> geometryShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> computeShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> computeShaderBlob;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader> createComputeShader;
	Microsoft::WRL::ComPtr<ID3D10Blob> createComputeShaderBlob;


	Microsoft::WRL::ComPtr<ID3D11Buffer> particlesBuffer;//one will be the updated
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlesUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> particlesBuffer2;//one will be the current
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlesUAV2;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indArgsBuffer;//one will be the current

	int findUnused();
};
