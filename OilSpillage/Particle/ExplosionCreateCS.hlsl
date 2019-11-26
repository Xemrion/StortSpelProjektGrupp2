struct Particle
{
    float4 position;
    float4 direction;
    float2 time;
};

AppendStructuredBuffer<Particle> NewSimulationState : register(u0);

cbuffer ParticleParameters : register(b0)
{
    float4 emitterLocation; //w = totalTime
    float4 randomVector; //w = size
    float4 initialDirection;
};

static const float3 direction[8] =
{
    normalize(float3(1.0f, 1.0f, 1.0f)),
	normalize(float3(-1.0f, 1.0f, 1.0f)),
	normalize(float3(-1.0f, -1.0f, 1.0f)),
	normalize(float3(1.0f, -1.0f, 1.0f)),
	normalize(float3(1.0f, 1.0f, -1.0f)),
	normalize(float3(-1.0f, 1.0f, -1.0f)),
	normalize(float3(-1.0f, -1.0f, -1.0f)),
	normalize(float3(1.0f, -1.0f, -1.0f))
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
void main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    Particle p;

    p.position.xyz = emitterLocation.xyz; 
    p.position.w = randomVector.w;
    p.direction = normalize(float4(reflect(direction[DispatchThreadID.x], hash(randomVector.xyz * 5.1 + float3(DispatchThreadID.x * 2.3f, DispatchThreadID.x * 1.4, DispatchThreadID.x * 4.3))) * 1.f, randomVector.w));
    p.direction *= 1.0f;
    p.time.x = 0.0f;
    p.time.y = emitterLocation.w;
    NewSimulationState.Append(p);
}