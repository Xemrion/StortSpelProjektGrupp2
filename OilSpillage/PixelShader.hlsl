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

float shadowVisible(float4 shadowPosition, Texture2D shadowMap, float biasTemp)
{
	float4 shadowCoord = shadowPosition;
	shadowCoord.x = shadowPosition.x / shadowPosition.w / 2.0f + 0.5f;
	shadowCoord.y = -shadowPosition.y / shadowPosition.w / 2.0f + 0.5f;
	shadowCoord.z = shadowCoord.z / shadowCoord.w;
	float visibility = 0.0f;
	float bias  = 0.0f;
	bias = biasTemp;


	int width;
	int height;
	int nrOfLevels;
	shadowMap.GetDimensions(0, width, height, nrOfLevels);
	float2 textureSize = float2(width, height);
	float2 texelSize = 1.0 / textureSize;
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = shadowMap.Sample(ShadowSamp, shadowCoord.xy + float2(x, y) * texelSize).r;
			visibility += shadowCoord.z - bias > pcfDepth ? 1.0f : 0.0;
		}
	}
	
	visibility /= 9.0;

	return visibility;
};
float4 main(VS_OUT input) : SV_Target
{
	float3 normal = input.NormalWS.xyz;
	float3 tangent = input.TangentWS.xyz;
	float3 bitangent = input.BitangentWS.xyz;
	float3 normalMap = NormalMap.Sample(SampSt, input.Tex).xyz;
	float4 specularColor = SpecularMap.Sample(SampSt, input.Tex);
	float gloss = GlossMap.Sample(SampSt, input.Tex).x;
	gloss = exp2(10 * gloss + 1);

	if (length(normalMap) > 0.f)
	{
		normalMap = 2.0f * normalMap - 1.0f;
		float3x3 TBN = float3x3(tangent, bitangent, normal);
		normal = normalize(mul(normalMap, TBN));
	}

	float4 texColor = Tex.Sample(SampSt, input.Tex).xyzw;

	uint2 lightTileIndex = floor(uint2(input.Pos.x, input.Pos.y) / uint2(16.f, 16.f));
	TileData lightTileData = tileData[lightTileIndex.y * 80 + lightTileIndex.x];

	float4 ambient = max(-dot(sunDir, normal)*(1- shadowVisible(input.shadowPos, ShadowMap, 0.00025f)), float4(0.1f, 0.1f, 0.1f, 1.0)) * sunColor;
	float shadowSpotVisible = 1.0f;

	float4 diffuseLight = float4(0.0, 0.0, 0.0, 1.0);
	float4 specularLight = float4(0.0, 0.0, 0.0, 0.0);

	for (int i = 0; i < lightTileData.numLights; ++i)
	{
		Light l = lights[lightTileData.indices[i]];
		float3 lightVector = l.pos.xyz - input.wPos.xyz;
		float attenuation = l.color.w / dot(lightVector, lightVector);
		if (attenuation < 0.0005) attenuation = 0;
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
				shadowSpotVisible = 1 - shadowSpotVisible;
			}
		}

		diffuseLight.rgb += max(l.color.rgb * nDotL * attenuation * directional * shadowSpotVisible, 0.0);
		float3 cameraVector = normalize(cameraPos.xyz - input.wPos.xyz);
		float3 halfway = normalize(normalize(lightVector) + cameraVector);
		specularLight.rgb += l.color.rgb * pow(max(dot(normal, halfway), 0.0), gloss) * attenuation * directional * shadowSpotVisible;
	}

	float4 outColor = (texColor + color) * (diffuseLight + ambient);
	outColor += (specularColor + color) * specularLight;

	return outColor;
}
