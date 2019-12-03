struct Particle
{
	float4 position;
	float4 velocity;
	float2 time;//.x = time, .y = totalTime
};
AppendStructuredBuffer<Particle> NewSimulationState : register(u0);
ConsumeStructuredBuffer<Particle> CurrentSimulationState : register(u1);

Texture2D DepthTexture : register(t0);
SamplerState Sampler: register(s0);
cbuffer SimulationParams : register(b0)
{
	float4 TimeFactors;
	float4 VectorField;//.x = moveSine, .y = vectorFieldPower .z = vectorFieldSize
	float4 ConsumerLocation;
};
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
		Particle p = CurrentSimulationState.Consume();
        NewSimulationState.Append(p);
       
		//p.time.x = p.time.x + TimeFactors.x;
		//if (p.time.x < p.time.y)
		//{
		//	NewSimulationState.Append(p);
		//}

	}

}