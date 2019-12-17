struct Particle
{
    float4 position;
    float4 velocity;
    float2 time; //.x = time, .y = totalTime
};
ConsumeStructuredBuffer<Particle> CurrentSimulationState : register(u0);
cbuffer ParticleCount : register(b1)
{
    uint4 NumParticles;
};

[numthreads(512, 1, 1)]

void main(uint3 DTid : SV_DispatchThreadID)
{
    uint myID = DTid.x + DTid.y * 512 + DTid.z * 512 * 512;
    if (myID < NumParticles.x && NumParticles.x > 0)
    {
        CurrentSimulationState.Consume();//consume all live particles
    }
}