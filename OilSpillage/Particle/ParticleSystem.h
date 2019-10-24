#pragma once
#include<d3d11.h>
#include<wrl/client.h>
#include<d3dcompiler.h>
#include"..///DynamicCamera.h"
using namespace DirectX::SimpleMath;
struct Particle
{
	Vector4 position;
	Vector4 direction;
	Vector2 time;//time and totalTime
};
struct CameraInfo
{
	Matrix viewProj;
	Vector4 camPos;
	Vector4 upp;
};
struct ParticleParams
{
	Vector4 emitterLocation;
	Vector4 randomVector;
	Vector4 initialDirection;
};
struct ParticleRenderParams
{
	Vector4 colors[4];
	Vector4 config;
};
struct SimulationParams
{
	Vector4 timeFactors;
	Vector4 vectorField;
	Vector4 consumerLocation;
};
struct IndirDraw
{
	UINT vertexCount;//particle count
	UINT instanceCount;//1
	UINT vertexStartLoc;
	UINT instanceStartLoc;
};
class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();
	void initiateParticles(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const wchar_t* csUpdate, const wchar_t* csCreate, const wchar_t* gs);
	bool addParticle(int nrOf, float lifeTime, Vector3 position, Vector3 initialDirection);
	void updateParticles(float delta, Matrix viewProj);
	void changeColornSize(Vector4 colors[4], int nrOfColors, float startSize, float endSize);
	void changeVectorField(float vectorFieldPower, float vectorFieldSize);
	void drawAll(DynamicCamera* camera);
private:
	const int capParticle = 51200 * 2.0;//100*512
	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	int nrOfParticles = 0;
	float otherFrame = 1.0f;
	int lastUsedParticle;
	int firstAdd = 0;
	float deltaTime = 0.0f;
	float sinMovement = 0.0f;
	IndirDraw indDraw;
	ParticleRenderParams colorNSize;
	ParticleParams pParams;
	SimulationParams sP;

	//Particle* particles;
	//ID3D11Buffer* particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> particleParamCB;//For compshader
	Microsoft::WRL::ComPtr<ID3D11Buffer> particleParamRenderCB;//For the draw
	Microsoft::WRL::ComPtr<ID3D11Buffer> viewProjBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> nrOfParticlesCB;
	Microsoft::WRL::ComPtr<ID3D11Buffer> simParams;//for update
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;//for update

	ID3D11ShaderResourceView* depthSRV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> collisionViewProj;

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
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particlesSRV;

	Microsoft::WRL::ComPtr<ID3D11Buffer> particlesBuffer2;//one will be the current
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> particlesUAV2;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> particlesSRV2;

	Microsoft::WRL::ComPtr<ID3D11Buffer> indArgsBuffer;//one will be the current

};
