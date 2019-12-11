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
    float4 initialDirection; // .w=size
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
    float3 earlyPoint = emitterLocation.xyz;
    p.position.xyz = emitterLocation.xyz; //+ 0.1f*normalize(DispatchThreadID.x * direction[DispatchThreadID.x]);
    p.position.w = 1.0f;
    float3 initDir = randomVector.xyz;
    p.direction.xyz = initDir.xyz;
    p.direction.w = 4.0f;
    p.time.x = 0.0f;
    p.time.y = emitterLocation.w;
    NewSimulationState.Append(p);
    float scale = 25.0f;
    scale = initialDirection.w;
    float spacing = 0.2f;
    spacing = initialDirection.z;
    for (int i = 1; i < scale; i++)
    {
        float3 randomVec = 2.0f*direction[DispatchThreadID.x] + hash(i * 2.0f + DispatchThreadID.x * randomVector.xyz) + hash(randomVector.xyz);
        normalize(randomVec);
        earlyPoint += spacing * randomVec;
        p.position.xyz = earlyPoint.xyz;
        p.position.y = emitterLocation.y;
        p.direction.xyz = randomVec.xyz;
        NewSimulationState.Append(p);
        if (hash(randomVector.xyz*DispatchThreadID.x).x > 0.1f)
        {
            earlyPoint += spacing * randomVec;
            p.position.xyz = earlyPoint.xyz;
            p.position.y = emitterLocation.y;
            p.direction.xyz = randomVec.xyz;
            NewSimulationState.Append(p);
        }
    }
}