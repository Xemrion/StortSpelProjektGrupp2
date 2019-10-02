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
float3 hash(float3 p)
{
	p = float3(
		dot(float3(9.423, 8.4212, 4.32), p),
		dot(float3(95.321, -59.321, -32.5289), p),
		dot(float3(-37.442, 74.32, 65.234), p));

	return 1.0 - 2.0 * frac(sin(p) * 9528);
}
static const float G = 9.82f;
static const float m1 = 0.1f;
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
		float3 wind = float3(1.0f, 0.0f, 0.5f);
		float3 random = hash(float3(43.5, 12.322, 21.5));
		float windPower = 1.0f;
		wind = wind * windPower + random * 0.5f;
		float3 acceleration = G * m1 * float3(0, -1, 0) * TimeFactors.x;
		acceleration += (wind / m1) * TimeFactors.x;
		if (p.position.y > depth)
		{
			p.velocity.xyz = p.velocity.xyz + acceleration;
			//p.velocity.xyz+= (hash(p.position.xxz) * 0.5f) + hash(p.position.xzz);
			p.position.xyz = p.position.xyz + p.velocity.xyz * TimeFactors.x;
		}
		else
		{
			p.velocity.xyz = (p.velocity.xyz + TimeFactors.x * float3(0, -1, 0))/(1 + (friction* TimeFactors.x));
			//p.velocity.xyz += (hash(p.position.xxz) * 0.5f) + hash(p.position.xzz);
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