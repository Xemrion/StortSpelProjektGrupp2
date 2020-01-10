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
    ParticleParams addArr[32];
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
    float3 earlyPoint = addArr[GroupID.x].emitterLocation.xyz;
    p.position.xyz = addArr[GroupID.x].emitterLocation.xyz; //+ 0.1f*normalize(DispatchThreadID.x * direction[DispatchThreadID.x]);
    p.position.w = 1.0f;
    float3 initDir = addArr[GroupID.x].randomVector.xyz;
    p.direction.xyz = initDir.xyz;
    p.direction.w = 4.0f;
    p.time.x = 0.0f;
    p.time.y = addArr[GroupID.x].emitterLocation.w;
    NewSimulationState.Append(p);
    float scale = 25.0f;
    scale = addArr[GroupID.x].initialDirection.w;
    float spacing = 0.2f;
    spacing = addArr[GroupID.x].initialDirection.z;
    for (int i = 1; i < scale; i++)
    {
        float3 randomVec = 2.0f * direction[GroupThreadID.x] + hash(i * 2.0f + GroupThreadID.x * addArr[GroupID.x].randomVector.xyz) + hash(addArr[GroupID.x].randomVector.xyz);
        normalize(randomVec);
        earlyPoint += spacing * randomVec;
        p.position.xyz = earlyPoint.xyz;
        p.position.y = addArr[GroupID.x].emitterLocation.y;
        p.direction.xyz = randomVec.xyz;
        NewSimulationState.Append(p);
        if (hash(addArr[GroupID.x].randomVector.xyz * GroupThreadID.x).x > 0.1f)
        {
            earlyPoint += spacing * randomVec;
            p.position.xyz = earlyPoint.xyz;
            p.position.y = addArr[GroupID.x].emitterLocation.y;
            p.direction.xyz = randomVec.xyz;
            NewSimulationState.Append(p);
        }
    }
}