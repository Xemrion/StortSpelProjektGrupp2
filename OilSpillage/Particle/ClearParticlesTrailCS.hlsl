struct Particle
{
    float4 position;
    float4 velocity;
    float2 time; //.x = time, .y = totalTime
};
globallycoherent RWStructuredBuffer<Particle> CurrentSimulationState : register(u0);
cbuffer ParticleCount : register(b0)
{
    uint4 NumParticles;
};

[numthreads(512, 1, 1)]

void main(uint3 DTid : SV_DispatchThreadID)
{
    uint myID = DTid.x + DTid.y * 512 + DTid.z * 512 * 512;
    if (myID < NumParticles.x && NumParticles.x > 0)
    {
        Particle p;
        p.position = float4(0, 0, 0, 0);
        p.time = float2(0, 0);
        p.velocity = float4(0, 0, 0, 0);
        CurrentSimulationState[myID] = p;
        CurrentSimulationState.DecrementCounter();
        
    }
}