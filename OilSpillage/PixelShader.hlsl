#include "ShaderDefines.hlsli"

struct VS_OUT
{
	float4 Pos: SV_POSITION;
	float4 wPos: APOS;
	float2 Tex : TEXCOORD;
	float4 NormalWS : NORMAL;
	float4 TangentWS : TANGENT;
	float4 BitangentWS : BINORMAL;
};

struct Light
{
	float4 pos;
	float4 color;
	float4 directionWidth;
};

struct TileData
{
	uint numLights;
	uint indices[MAX_LIGHTS_PER_TILE];
};

cbuffer MaterialBuffer : register(b0)
{
	float4 color;
}

cbuffer Lights : register(b1)
{
	Light lights[MAX_LIGHTS_TOTAL];
}

cbuffer SunInfo : register(b2) {
	float4 sunColor;
	float4 sunDir;
};

Texture2D Tex : register(t0);
Texture2D NormalMap : register(t1);
SamplerState SampSt : register(s0);
StructuredBuffer<TileData> tileData : register(t2);

float4 main(VS_OUT input) : SV_Target
{
	float3 normal = input.NormalWS.xyz;
	float3 tangent = input.TangentWS.xyz;
	float3 bitangent = input.BitangentWS.xyz;
	float3 normalMap = NormalMap.Sample(SampSt, input.Tex).xyz;
	
	if (length(normalMap) > 0.f)
	{
		normalMap = 2.0f * normalMap - 1.0f;
		float3x3 TBN = float3x3(tangent, bitangent, normal);
		normal = normalize(mul(normalMap, TBN));
	}

	float4 texColor = Tex.Sample(SampSt, input.Tex).xyzw;

	uint2 lightTileIndex = floor(uint2(input.Pos.x, input.Pos.y) / uint2(16.f, 16.f));
	TileData lightTileData = tileData[lightTileIndex.y * 80 + lightTileIndex.x];

	float4 ambient = max(-dot(sunDir, normal), float4(0.2, 0.2, 0.2, 1.0)) * sunColor;
	
	float4 diffuse = float4(0.0, 0.0, 0.0, 1.0);
	for (int i = 0; i < lightTileData.numLights; ++i)
	{
		Light l = lights[lightTileData.indices[i]];
		float3 lightVector = l.pos.xyz - input.wPos.xyz;
		float attenuation = l.color.w / dot(lightVector, lightVector);
		if (attenuation < 0.0005) attenuation = 0;
		float nDotL = max(dot(normal, normalize(lightVector)), 0.0);
		float directional = 1.0;

		//if the light is a spot light
		if (l.directionWidth.w > 0.0)
		{
			directional = 0.0;
			float s = dot(-normalize(lightVector), l.directionWidth.xyz);
			float umbra = cos(l.directionWidth.w);
			if (s > umbra) {
				float penumbra = cos(l.directionWidth.w * 0.9);
				directional = (s - umbra) / (penumbra - umbra);
				directional *= directional;
			}
		}

		diffuse.rgb += max(l.color.xyz * nDotL * attenuation * directional, 0.0);
	}

	float4 outColor = (texColor + color) * (diffuse + ambient);
	//return float4(normal, 1.0);
	return outColor / (outColor + float4(1.0, 1.0, 1.0, 0.0));
}
