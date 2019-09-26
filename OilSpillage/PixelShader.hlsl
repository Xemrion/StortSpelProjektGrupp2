#include "ShaderDefines.hlsli"

struct VS_OUT
{
	float4 Pos: SV_POSITION;
	float4 wPos: APOS;
	float2 Tex : TEXCOORD;
	float4 NormalWS : NORMAL;
};

cbuffer CB_COLOR : register(b0)
{
	float4 color;
}

cbuffer SunInfo : register(b2) {
	float4 sunColor;
	float4 sunDir;
};

struct Light
{
	float4 pos;
	float4 color;
	float4 directionWidth;
};

Texture2D Tex:register(t0);
SamplerState SampSt :register(s0);
StructuredBuffer<Light> lights : register(t1);

float4 main(VS_OUT input) : SV_Target
{
	float3 normal = input.NormalWS.xyz;
	float4 texColor = Tex.Sample(SampSt, input.Tex).xyzw;

	float4 ambient = max(-dot(sunDir, normal), float4(0.2, 0.2, 0.2, 1.0)) * sunColor;

	float4 diffuse = float4(0.0, 0.0, 0.0, 1.0);
	for (int i = 0; i < MAX_LIGHTS_ON_SCREEN; ++i) 
	{
		float3 lightVector = lights[i].pos.xyz - input.wPos.xyz;
		float attenuation = lights[i].color.w / dot(lightVector, lightVector);
		float nDotL = max(dot(normal, normalize(lightVector)), 0.0);
		float directional = 1.0;

		//if the light is a spot light
		if (lights[i].directionWidth.w > 0.0)
		{
			directional = 0.0;
			float s = dot(-normalize(lightVector), lights[i].directionWidth.xyz);
			float umbra = cos(lights[i].directionWidth.w);
			if (s > umbra) {
				float penumbra = cos(lights[i].directionWidth.w * 0.9);
				directional = (s - umbra) / (penumbra - umbra);
				directional *= directional;
			}
		}

		diffuse.xyz += max(lights[i].color.xyz * nDotL * attenuation * directional, 0.0);
	}

	float4 outColor = (texColor + color) * (diffuse + ambient);
	return outColor / (outColor + float4(1.0, 1.0, 1.0, 0.0));
}
