#include "ShaderDefines.hlsli"

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 wPos: APOS;
	float2 Tex : TEXCOORD;
	float4 NormalWS : NORMAL;
	float4 shadowPos : SHADOWPOS;
	float4 shadowPosSpot : SHADOWPOSSPOT;
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

cbuffer SpotLightIndex : register(b3)
{
	uint indexForSpot;
};

cbuffer CameraInfo : register(b4)
{
	float4 cameraPos;
}


Texture2D Tex : register(t0);
Texture2D NormalMap : register(t1);
Texture2D ShadowMap : register(t3);
Texture2D ShadowMapSpot : register(t4);
Texture2D SpecularMap : register(t5);
Texture2D GlossMap : register(t6);
SamplerState SampSt : register(s0);
SamplerState ShadowSamp : register(s1);
StructuredBuffer<TileData> tileData : register(t2);

float shadowVisible(float4 shadowPosition, Texture2D shadowMap, float bias)
{
	float4 shadowCoord = shadowPosition;
	shadowCoord.xyz /= shadowCoord.w;
	shadowCoord.xy = shadowCoord.xy * float2(0.5f, -0.5f) + 0.5f;
	float visibility = 0.0f;

	float4 pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(0, 0)).r;
	visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;

	pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(-2, -0)).r;
	visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;

	pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(-2, 0)).r;
	visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;

	pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(0, -2)).r;
	visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
	visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;

	visibility *= 0.25 * 0.25;

	return visibility;
};
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
	
	uint2 lightTileIndex = uint2(input.Pos.x * 0.0625f, input.Pos.y * 0.0625f);
	TileData lightTileData = tileData[lightTileIndex.y * 80 + lightTileIndex.x];

	float4 ambient = max(-dot(sunDir, normal) * (1 - shadowVisible(input.shadowPos, ShadowMap, 0.00015f) * texColor.a), float4(0.2f, 0.2f, 0.2f, 1.0)) * sunColor;
	float shadowSpotVisible = 1.0f;

	float4 diffuseLight = float4(0.0, 0.0, 0.0, 1.0);

	for (int i = 0; i < lightTileData.numLights; ++i)
	{
		Light l = lights[lightTileData.indices[i]];
		float3 lightVector = l.pos.xyz - input.wPos.xyz;
		float attenuation = l.color.w / dot(lightVector, lightVector);

		float nDotL = max(dot(normal, normalize(lightVector)), 0.0);
		float directional = 1.0;
		shadowSpotVisible = 1.0f;
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
			if (lightTileData.indices[i] == indexForSpot)
			{
				shadowSpotVisible = shadowVisible(input.shadowPosSpot, ShadowMapSpot, 0.005f);
				shadowSpotVisible = 1 - shadowSpotVisible * texColor.a;
			}
		}

		diffuseLight.rgb += max(l.color.rgb * nDotL * attenuation * directional * shadowSpotVisible, 0.0);
	}

	float4 outColor = (texColor + color) * (diffuseLight + ambient);

	return outColor;
}
