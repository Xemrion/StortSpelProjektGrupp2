#include "ShaderDefines.hlsli"

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 wPos: APOS;
	float2 Tex : TEXCOORD;
	float4 NormalWS : NORMAL;
	float4 shadowPos : SHADOWPOS;
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

cbuffer CB_COLOR : register(b0)
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
Texture2D ShadowMap : register(t2);
SamplerState SampSt : register(s0);
SamplerState ShadowSamp : register(s1);
StructuredBuffer<TileData> tileData : register(t1);

float4 main(VS_OUT input) : SV_Target
{
	float3 normal = input.NormalWS.xyz;
	float4 texColor = Tex.Sample(SampSt, input.Tex).xyzw;
	uint2 lightTileIndex = floor(uint2(input.Pos.x, input.Pos.y) / uint2(16.f, 16.f));
	TileData lightTileData = tileData[lightTileIndex.y * 80 + lightTileIndex.x];


	float4 shadowCoord = input.shadowPos;
	shadowCoord.z = shadowCoord.z / shadowCoord.w;
	shadowCoord.xy = (0.5f * shadowCoord.xy) + 0.5f;
	shadowCoord.y = abs(shadowCoord.y - 1);
	float visibility = 0.0;
	float bias;
	bias = max(0.01 * (1.0 - dot(normal, sunDir.xyz)), 0.005);
	bias = 0.00025f;

	int width;
	int height;
	int nrOfLevels;
	ShadowMap.GetDimensions(0, width, height, nrOfLevels);
	float2 textureSize = float2(width, height);
	float2 texelSize = 1.0 / textureSize;
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = ShadowMap.Sample(ShadowSamp, shadowCoord.xy + float2(x, y) * texelSize).r;
			visibility += shadowCoord.z - bias > pcfDepth ? 1.0f : 0.0;
		}
	}
	visibility /= 9.0;
	
	if (shadowCoord.z > 1.0)
		visibility = 0.0f;

	float4 ambient = max(-dot(sunDir, normal)*(1-visibility), float4(0.2, 0.2, 0.2, 1.0)) * sunColor;
	
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
	return outColor;
	return outColor / (outColor + float4(1.0, 1.0, 1.0, 0.0));
}
