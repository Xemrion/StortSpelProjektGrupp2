struct Particle
{
	float3 position;
	float3 direction;
	float time;
};

AppendStructuredBuffer<Particle> NewSimulationState : register(u0);

cbuffer ParticleParameters : register(b0)
{
	float4 emitterLocation;
	float4 randomVector;
};

static const float3 direction[8] =
{
	normalize(float3(1.0f , 1.0f,1.0f)),
	normalize(float3(-1.0f, 1.0f,1.0f)),
	normalize(float3(-1.0f, -1.0f,1.0f)),
	normalize(float3(1.0f,  -1.0f,1.0f)),
	normalize(float3(1.0f,  1.0f,-1.0f)),
	normalize(float3(-1.0f, 1.0f,-1.0f)),
	normalize(float3(-1.0f, -1.0f,-1.0f)),
	normalize(float3(1.0f,  -1.0f,-1.0f))
};
[numthreads(8, 1, 1)]
void main( uint3 GroupThreadID : SV_GroupThreadID )
{
	Particle p;

	p.position = emitterLocation.xyz+ float3(GroupThreadID.x+1,0,0);

	p.direction = reflect(direction[GroupThreadID.x], randomVector.xyz) * 5.0f;

	p.time = 0.0f;

	NewSimulationState.Append(p);
}