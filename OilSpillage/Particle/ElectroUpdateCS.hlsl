struct Particle
{
    float4 position;
    float4 velocity;
    float2 time; //.x = time, .y = totalTime
};
AppendStructuredBuffer<Particle> NewSimulationState : register(u0);
ConsumeStructuredBuffer<Particle> CurrentSimulationState : register(u1);
Texture2D DepthTexture : register(t0);
SamplerState Sampler : register(s0);
cbuffer SimulationParams : register(b0)
{
    float4 TimeFactors;
    float4 VectorField; //.x = moveSine, .y = vectorFieldPower .z = vectorFieldSize
    float4 ConsumerLocation;
};
cbuffer ParticleCount : register(b1)
{
    uint4 NumParticles;
};
cbuffer CameraMatrix : register(b2)
{
    float4x4 viewProj;
};
float random(float2 arg)
{
    return frac(sin(dot(arg.xy, float2(13.5123, 48.321))) * 52323.457821);
}
float3 hash(float3 p)
{
    p = float3(
		dot(float3(9.423, 8.4212, 4.32), p),
		dot(float3(95.321, -59.321, -32.5289), p),
		dot(float3(-37.442, 74.32, 65.234), p));

    return 1.0 - 2.0 * frac(sin(p) * 9528);
}
float noise(float2 pos)
{
    float2 i = floor(pos);
    float2 f = frac(pos);

    float4 abcd = float4(random(i), random(i + float2(1.0f, 0.0f)), random(i + float2(0.0f, 1.0f)), random(i + float2(1.0f, 1.0f)));

    float2 smooth = smoothstep(0.0f, 1.0f, f); // 0 - 1 

    return 1.0f;
}
static const float G = 9.82f;
static const float m1 = 0.5f;

[numthreads(512, 1, 1)]

void main(uint3 DTid : SV_DispatchThreadID)
{
    uint myID = DTid.x + DTid.y * 512 + DTid.z * 512 * 512;
    if (myID < NumParticles.x && NumParticles.x > 0)
    {
        Particle p = CurrentSimulationState.Consume();
        float moveSine = 0.0f;
        float fieldPower = 1.0f;
        float fieldSize = 1.0f;
        float3 acceleration = 0.0f;
        float xSin = sin((sin(-5.8f * moveSine - 1) + p.position.x) * (1 / fieldSize) - (moveSine * 8.2f));
        float ySin = 0.0f;
        float zSin = sin((sin(-4.8f * moveSine - 2) + p.position.z) * (1 / fieldSize) - (moveSine * 7.7f));
        acceleration += ((2.85f * fieldPower * normalize(float3(xSin, ySin, zSin)))) * TimeFactors.x;

        p.velocity.xyz = p.velocity.xyz + acceleration;
     
        p.position.xyz = p.position.xyz + p.velocity.xyz * TimeFactors.x;
        p.time.x = p.time.x + TimeFactors.x;
        if (p.time.x < p.time.y)
        {
            NewSimulationState.Append(p);
        }
		
    }

}