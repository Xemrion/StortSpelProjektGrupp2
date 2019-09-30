struct Particle
{
	float3 position;
	float4 direction;
	float4 color;
	float2 time;
};

AppendStructuredBuffer<Particle> NewSimulationState : register(u0);

cbuffer ParticleParameters : register(b0)
{
	float4 emitterLocation;//w = totalTime
	float4 randomVector;
	float4 initialDirection;
	float4 color;
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

float3 hash(float3 p)
{
	p = float3(
		dot(float3(9.423, 8.4212, 4.32), p),
		dot(float3(95.321, -59.321, -32.5289), p),
		dot(float3(-37.442, 74.32, 65.234), p));

	return 1.0 - 2.0 * frac(sin(p) * 9528);
}
[numthreads(8, 1, 1)]
void main( uint3 DispatchThreadID : SV_DispatchThreadID )
{
	Particle p;

	p.position = emitterLocation.xyz;/*+sin(GroupThreadID.x*10)+cos(GroupThreadID.x*6)+ direction[GroupThreadID.x];*/

	p.direction = float4(reflect(direction[DispatchThreadID.x], hash(randomVector.xyz + float3(DispatchThreadID.x * 2.3f, DispatchThreadID.x * 1.4, DispatchThreadID.x * 4.3))) * 1.f, 0.0f);
	p.direction += float4(initialDirection.xyz, 0.0f);
	p.color = float4(1.0f, 0.2f, 0.2f, 1.0f);//red;
	p.time.x = 0.0f;
	p.time.y = emitterLocation.w;
	NewSimulationState.Append(p);
}