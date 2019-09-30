struct Particle
{
	float3 position;
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
static const float m1 = 0.003f;
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


		float size1 = 0.00f;
		float size2 = 0.15;
		float halfTime = p.time.y / 3.0f;

		float deltaSize = (size2 - size1) / (p.time.y);
		if (p.time.x < halfTime)
		{
			p.velocity.w += deltaSize * TimeFactors.x;
		}
		else
		{
			p.velocity.w -= deltaSize * TimeFactors.x;
		}
		if (p.velocity.w <= 0)
		{
			p.velocity.w = 0;
		}

		float4 color1 = float4(0.7f, 0.0f, 0.0f, 1.0f);//red
		float4 color2 = float4(0.7f, 0.0f, 0.0f, 1.0f);//blue
		float4 color3 = float4(0.0f, 0.0f, 0.0f, 1.0f);
		float4 color4 = float4(0.1f, 0.1f, 0.1f, 1.0f);
		float4 deltaColor = (color2 - color1) / halfTime;
		float4 deltaColor2 = (color3 - color2) / halfTime;
		float4 deltaColor3 = (color4 - color3) / halfTime;

		float4 particleColor; 
		particleColor = float4(deltaColor * TimeFactors.x);
		if (p.time.x >= halfTime)
		{
			particleColor= float4(deltaColor2 * TimeFactors.x);
		}
		if (p.time.x >= halfTime*2)
		{
			particleColor = float4(deltaColor3 * TimeFactors.x);
		}
		float depth = 0.0f;

		if (p.position.y > depth)
		{
			p.velocity.xyz = p.velocity.xyz + G  * m1 * TimeFactors.x * float3(0, -1, 0);
			p.position = p.position + p.velocity.xyz * TimeFactors.x;
		}
		
		p.time.x = p.time.x + TimeFactors.x;
		p.color += particleColor;
		if (p.time.x < p.time.y)
		{
			NewSimulationState.Append(p);
		}
		
	}

}