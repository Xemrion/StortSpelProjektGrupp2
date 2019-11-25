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

[numthreads(512, 1, 1)]

void main( uint3 DTid : SV_DispatchThreadID)
{
	uint myID = DTid.x + DTid.y * 512 + DTid.z * 512 * 512;
	if (myID < NumParticles.x && NumParticles.x > 0)
	{
		Particle p = CurrentSimulationState.Consume();
		
        float depth = -1.4f;
		float friction = 0.05f;
		float3 wind = float3(1.0f, 0.0f, 0.5f);
		float3 random = hash(float3(43.5, 12.322, 21.5));
		float windPower = 0.0f;
		wind = wind * windPower + random * 0.0f;
		float3 acceleration = 1.0f*G * m1 * float3(0, -1, 0) * TimeFactors.x;
		acceleration += (wind / m1) * TimeFactors.x;
		float moveSine = VectorField.x;//increases every frame to move the sine
		float fieldSize = VectorField.z;//for the vectorField
		float fieldPower = VectorField.y;//for the vectorField
        float xSin = sin((sin(-5.8f * moveSine - 1) + p.position.x) * (1 / fieldSize) - (moveSine * 8.2f));
        float ySin = sin((sin(-7.8f * moveSine + 1) + p.position.y) * (1 / fieldSize) - (moveSine * 0.7f));
        float zSin = sin((sin(-4.8f * moveSine - 2) + p.position.z) * (1 / fieldSize) - (moveSine * 7.7f));
		acceleration += ((2.85f * fieldPower * normalize(float3(xSin, ySin, zSin)))) * TimeFactors.x;
		if (p.position.y > depth)
		{
			p.velocity.xyz = p.velocity.xyz + acceleration;
		}
		else
		{
			//On zero or lower apply friction and set velocity.y to 0 for no movement in y 
			p.velocity.xyz = (p.velocity.xyz + TimeFactors.x * float3(0, -1, 0))/(1 + (friction* TimeFactors.x));
			p.velocity.y = 0;
           // p.velocity.xyz = 1.1f*reflect(p.velocity.xyz, float3(0.0f, 1.0f, 0.0f))+acceleration;
            //+reflect(p.velocity.xyz + acceleration, float3(0.0f, 1.0f, 0.0f)); 
        }

		p.position.xyz = p.position.xyz + p.velocity.xyz * TimeFactors.x;
		p.time.x = p.time.x + TimeFactors.x;
		if (p.time.x < p.time.y)
		{
			NewSimulationState.Append(p);
		}
		
	}

}