struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
};

struct PS_OUT
{
	float4 color;
};

cbuffer MaterialBuffer : register(b0)
{
	float3 color;
	float densityThreshold;
	float density;
	float ambientDensity;
	float scale;
}

Texture2D randomLUT : register(t0);
SamplerState samplerState : register(s0);

float2 hash(float2 p)
{
	return randomLUT.Sample(samplerState, p / scale);
}

float noise(in float2 x)
{
	float2 i = floor(x);
	float2 f = frac(x);
	float2 u = f * f * f * (f * (f * 6.0 - 15.0) + 10.0);

	return lerp(lerp(dot(hash(i + float2(0.0, 0.0)), f - float2(0.0, 0.0)),
					 dot(hash(i + float2(1.0, 0.0)), f - float2(1.0, 0.0)), u.x),
				lerp(dot(hash(i + float2(0.0, 1.0)), f - float2(0.0, 1.0)),
					 dot(hash(i + float2(1.0, 1.0)), f - float2(1.0, 1.0)), u.x), u.y);
}

PS_OUT main(VS_OUT input) : SV_TARGET
{
	float n = (noise(input.Tex.xy * scale)* (1.0   - 0.0)/* +
		noise(input.Tex.xy * scale * 2.0) * (0.5   + 0.01) +
		noise(input.Tex.xy * scale * 4.0) * (0.25  - 0.01) +
		noise(input.Tex.xy * scale * 8.0) * (0.125 + 0.01)*/) *
		0.5 + 0.5;
	n = max(n - densityThreshold, 0.0) * (1.0 / (1.0 - densityThreshold));

	PS_OUT output;
	output.color = float4(color.r, color.g, color.b, max(n * density, ambientDensity));
	//output.color.a += randomLUT.Sample(samplerState, input.Tex.xy) * 0.005;
	return output;
}