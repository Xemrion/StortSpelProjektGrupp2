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
    float4 randomVector;
    float4 initialDirection; // trailDirection (opposite of carDir)
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

[numthreads(8, 1, 1)]
void main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
    Particle p;
    float frameID = initialDirection.w;
    float noiseValue = hash(32.03f).x*10.0f;
    float3 initDir = randomVector.xyz;
    normalize(initDir);
    float3 noiseDir = float3(hash(2.123f).y, hash(23.22f).z, hash(72.323f).x);
    p.position.xyz = emitterLocation.xyz; //+ 0.1f*normalize(DispatchThreadID.x * direction[DispatchThreadID.x]);
    p.position.w = 1.0f;
    p.direction.xyz = initDir.xyz;
    p.direction.w = 2.0f;
    p.time.x = 0.0f;
    p.time.y = emitterLocation.w;
    NewSimulationState.Append(p);
}