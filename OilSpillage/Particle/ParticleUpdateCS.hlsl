struct Particle
{
	float3 position;
	float3 velocity;
	float4 color;
	float time;
};
AppendStructuredBuffer<Particle> NewSimulationState : register(u0);
ConsumeStructuredBuffer<Particle> CurrentSimulationState : register(u1);

cbuffer SimulationParams : register(b0)
{
	float4 TimeFactors;
	float4 EmitterLocation;
	float4 ConsumerLocation;
};

cbuffer ParticleCount : register(b1)
{
	uint4 NumParticles;
};

static const float G = 9.82f;
static const float m1 = 0.1f;
static const float m2 = 10.0f;
static const float m1m2 = m1 * m2;
static const float eventHorizon = 1.0f;
[numthreads(1024, 1, 1)]

void main( uint3 DTid : SV_DispatchThreadID )
{
	uint myID = DTid.x + DTid.y * 1024 + DTid.z * 1024 * 1024;

	if (myID < NumParticles.x)
	{
		Particle p = CurrentSimulationState.Consume();

		/*float3 d = ConsumerLocation.xyz - p.position;
		float r = length(d);

		float3 force = (G * m1m2 / (r * r)) * normalize(d);

		p.velocity = p.velocity + (force / m1) * TimeFactors.x;

		p.position += p.velocity * TimeFactors.x;

		p.time = p.time + TimeFactors.x;*/

		p.velocity = p.velocity + G*0.05f * m1 * TimeFactors.x * float3(0,1,0);
		p.position = p.position + p.velocity * TimeFactors.x;
		p.time = p.time + TimeFactors.x;
		if (p.time < 10.0f)
		{
			NewSimulationState.Append(p);
		}
		
	}

}