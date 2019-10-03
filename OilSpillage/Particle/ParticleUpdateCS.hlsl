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

	float4 abcd = float4(random(i),random(i+float2(1.0f,0.0f)),random(i+float2(0.0f,1.0f)),random(i+float2(1.0f,1.0f)));

	float2 smooth = smoothstep(0.0f, 1.0f, f);// 0 - 1 

	return 1.0f;
}
static const float G = 9.82f;
static const float m1 = 0.5f;

[numthreads(1024, 1, 1)]

void main( uint3 DTid : SV_DispatchThreadID )
{
	uint myID = DTid.x + DTid.y * 1024 + DTid.z * 1024 * 1024;
	
	if (myID < NumParticles.x && NumParticles.x > 0)
	{
		Particle p = CurrentSimulationState.Consume();
		
		float depth = 0.0f;
		float friction = 0.05f;
		float3 wind = float3(1.0f, 0.0f, 0.5f);
		float3 random = hash(float3(43.5, 12.322, 21.5));
		float windPower = 0.2f;
		wind = wind * windPower + random * 0.0f;
		float3 acceleration = 1.0f*G * m1 * float3(0, -1, 0) * TimeFactors.x;
		//acceleration += (wind / m1) * TimeFactors.x;
		float moveSine = EmitterLocation.x;//increases every frame to move the sine
		acceleration += 0.75f*normalize(float3(sin((0.7f * p.position.x)-(moveSine*1.2f)),sin((0.6f * p.position.y) - (moveSine*0.7f)), sin(0.8f * p.position.z - (moveSine*1.7f)))) * TimeFactors.x;
		acceleration += 0.00005f*float3(noise(p.position.x), noise(p.position.y), noise(p.position.z));
		if (p.position.y > depth)
		{
			p.velocity.xyz = p.velocity.xyz + acceleration;
		}
		else
		{
			//On zero or lower apply friction and set velocity.y to 0 for no movement in y 
			p.velocity.xyz = (p.velocity.xyz + TimeFactors.x * float3(0, -1, 0))/(1 + (friction* TimeFactors.x));
			p.velocity.y = 0;
		}
		p.position.xyz = p.position.xyz + p.velocity.xyz * TimeFactors.x;
		p.time.x = p.time.x + TimeFactors.x;
		if (p.time.x < p.time.y)
		{
			NewSimulationState.Append(p);
		}
		
	}

}