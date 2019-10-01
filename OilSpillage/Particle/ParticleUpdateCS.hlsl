struct Particle
{
	float4 position;
	float4 velocity;
	float4 color;
	float2 time;//.x = time, .y = totalTime
};
AppendStructuredBuffer<Particle> NewSimulationState : register(u0);
ConsumeStructuredBuffer<Particle> CurrentSimulationState : register(u1);
Texture2D DepthTexture : register(t0);
SamplerState Sampler: register(s0);
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
cbuffer CameraMatrix : register(b2)
{
	float4x4 viewProj;
};
static const float G = 9.82f;
static const float m1 = 0.03f;
static const float m2 = 10.0f;
static const float m1m2 = m1 * m2;
static const float eventHorizon = 1.0f;

[numthreads(1024, 1, 1)]

void main( uint3 DTid : SV_DispatchThreadID )
{
	uint myID = DTid.x + DTid.y * 1024 + DTid.z * 1024 * 1024;
	
	if (myID < NumParticles.x && NumParticles.x>0)
	{
		Particle p = CurrentSimulationState.Consume();

		//either bigger and then smaller or start size and decrease over time
		float size1 = p.position.w;
		float size2 = 0.0f;
		float halfTime = p.time.y / 3.0f;

		float deltaSize = (size2 - size1) / (p.time.y);
		if (p.time.x < halfTime)
		{
			p.velocity.w += deltaSize * TimeFactors.x;
		}
		/*else
		{
			p.velocity.w -= deltaSize * TimeFactors.x;
		}*/
		if (p.velocity.w <= 0)
		{
			p.velocity.w = 0;
		}
		
		
		float depth = 0.0f;
		float friction = 0.05f;
		if (p.position.y > depth)
		{
			p.velocity.xyz = p.velocity.xyz + G  * m1 * TimeFactors.x * float3(0, -1, 0);
			p.position.xyz = p.position.xyz + p.velocity.xyz * TimeFactors.x;
		}
		else
		{
			p.velocity.xyz = (p.velocity.xyz + TimeFactors.x * float3(0, -1, 0))/(1 + (friction* TimeFactors.x));
			p.velocity.y = 0;
			p.position.xyz = p.position.xyz + p.velocity.xyz * TimeFactors.x;
		}
		p.velocity.w = lerp(0.5f, 0.0f, p.time.x / (p.time.y+20));
		p.time.x = p.time.x + TimeFactors.x;
		p.color = float4(1.0f,1.0f,1.0f,1.0f);
		if (p.time.x < p.time.y)
		{
			NewSimulationState.Append(p);
		}
		
	}

}