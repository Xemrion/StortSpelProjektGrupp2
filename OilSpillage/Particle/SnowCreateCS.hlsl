struct Particle
{
	float4 position;
	float4 direction;
	float2 time;
};

AppendStructuredBuffer<Particle> NewSimulationState : register(u0);
struct ParticleParams
{
    float4 emitterLocation; //w = totalTime
    float4 randomVector; //w = size
    float4 initialDirection;
};
cbuffer ParticleParameters : register(b0)
{
    ParticleParams addPatricle[32];
};

static const float3 direction[16] =
{
    normalize(float3(1.0f, 1.0f, 1.0f)),
	normalize(float3(-1.0f, 1.0f, 1.0f)),
	normalize(float3(-1.0f, -1.0f, 1.0f)),
	normalize(float3(1.0f, -1.0f, 1.0f)),
	normalize(float3(1.0f, 1.0f, -1.0f)),
	normalize(float3(-1.0f, 1.0f, -1.0f)),
	normalize(float3(-1.0f, -1.0f, -1.0f)),
	normalize(float3(1.0f, -1.0f, -1.0f)),
	normalize(float3(0.5f, 1.0f, 0.5f)),
	normalize(float3(-1.0f, 0.5f, 0.5f)),
	normalize(float3(-0.5f, -1.0f, 0.5f)),
	normalize(float3(1.0f, -0.5f, 0.5f)),
	normalize(float3(0.5f, 1.0f, -0.5f)),
	normalize(float3(-1.0f, 0.5f, -0.5f)),
	normalize(float3(-0.5f, -1.0f, -0.5f)),
	normalize(float3(1.0f, -0.5f, -0.5f))
};

float3 hash(float3 p)
{
	p = float3(
		dot(float3(649.423, 648.4212, -234.32), p),
		dot(float3(95.321, -159.321, -932.5289), p),
		dot(float3(-317.442, 794.32, 695.234), p));

	return 1.0 - 2.0 * frac(cos(p) * 9528.764565);
}
[numthreads(16, 1, 1)]
void main(uint3 GroupThreadID : SV_GroupThreadID, uint3 GroupID : SV_GroupID)
{
	Particle p;

    p.position.xyz = addPatricle[GroupID.x].emitterLocation.xyz;
    p.position.w = addPatricle[GroupID.x].randomVector.w;
    p.direction = float4(reflect(direction[GroupThreadID.x], hash(addPatricle[GroupID.x].randomVector.xyz * 5.1 + float3(GroupThreadID.x * 2.3f, GroupThreadID.x * 1.4, GroupThreadID.x * 4.3))) * 1.f, addPatricle[GroupID.x].randomVector.w) * addPatricle[GroupID.x].initialDirection.w;
    p.direction += float4(addPatricle[GroupID.x].initialDirection.xyz, 0.0f);
	p.time.x = 0.0f;
    p.time.y = addPatricle[GroupID.x].emitterLocation.w;
	NewSimulationState.Append(p);
}