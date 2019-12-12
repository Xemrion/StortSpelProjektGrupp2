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

struct PS_OUT
{
	float4 color;
	float4 depth;
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
	float4 shadeTrue;
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

	visibility *= 0.125;

	return visibility;
};

PS_OUT main(VS_OUT input) : SV_Target
{
	PS_OUT output;
	float4 texColor = Tex.Sample(SampSt, input.Tex).xyzw;
	float3 normal = input.NormalWS.xyz;
	float3 tangent = input.TangentWS.xyz;
	float3 bitangent = input.BitangentWS.xyz;
	float3 normalMap = NormalMap.Sample(SampSt, input.Tex).xyz;
	float4 specularColor = max(SpecularMap.Sample(SampSt, input.Tex), float4(0.04, 0.04, 0.04, 0.0));
	float gloss = GlossMap.Sample(SampSt, input.Tex).x;
	gloss = exp2(10.0 * gloss + 1);

	if (dot(normalMap, normalMap) > 0.f)
	{
		normalMap = 2.0f * normalMap - 1.0f;
		float3x3 TBN = float3x3(tangent, bitangent, normal);
		normal = normalize(mul(normalMap, TBN));
	}

	uint2 lightTileIndex = uint2(input.Pos.x * 0.0625f, input.Pos.y * 0.0625f);
	TileData lightTileData = tileData[lightTileIndex.y * 80 + lightTileIndex.x];
	float sunShadow = (1 - shadowVisible(input.shadowPos, ShadowMap, 0.0005f));
	float4 ambient = float4(float3(0.2f, 0.2f, 0.2f) * sunColor.rgb, 1.0);

	float shadowSpotVisible = 1.0f;
	float3 cameraVector = normalize(cameraPos.xyz - input.wPos.xyz);

	float4 diffuseLight = float4(0.0, 0.0, 0.0, 1.0);
	diffuseLight.rgb += -dot(sunDir, normal) * sunShadow * sunColor.rgb;

	float4 specularLight = float4(0.0, 0.0, 0.0, 1.0);

	{
		float3 halfway = normalize(normalize(-sunDir) + cameraVector);
		float nDotH = saturate(dot(normal, halfway));
		float glossTerm = ((gloss + 2) / 8) * pow(nDotH, gloss);
		float3 fresnelTerm = specularColor + (1.0f - specularColor) * pow(1.0f - nDotH, 5);
		specularLight.rgb += max(sunColor * sunShadow, ambient) * fresnelTerm * glossTerm * saturate(dot(normal, -sunDir));
	}

	for (int i = 0; i < lightTileData.numLights; ++i)
	{
		Light l = lights[lightTileData.indices[i]];
		float3 lightVector = l.pos.xyz - input.wPos.xyz;
		float attenuation;
		float nDotL = saturate(dot(normal, normalize(lightVector)));
		
		shadowSpotVisible = 1.0f;
		attenuation = dot(lightVector, lightVector);
		nDotL = max(dot(normal, normalize(lightVector)), 0.0);

		float s = dot(-normalize(lightVector), l.directionWidth.xyz);
		float umbra = cos(l.directionWidth.w);
		float penumbra = cos(l.directionWidth.w * 0.9);

		float directional = s > umbra ? ((s - umbra) / (penumbra - umbra)) : 0.0;
		directional = l.pos.w == 1.0 ? directional * directional : 1.0;

		if (lightTileData.indices[i] == indexForSpot)
		{
			shadowSpotVisible = 1 - shadowVisible(input.shadowPosSpot, ShadowMapSpot, 0.005f);
		}

		//if the light is a laser
		if (l.pos.w == 2.0)
		{
			float3 rayPointVector = (l.pos.xyz - input.wPos.xyz) - dot(l.pos.xyz - input.wPos.xyz, l.directionWidth.xyz) * l.directionWidth.xyz;
			float dist = dot(rayPointVector, rayPointVector);

			float startDist = attenuation;
			float3 endVector = (l.pos.xyz + l.directionWidth.xyz * l.directionWidth.w) - input.wPos.xyz;
			float endDist = dot(endVector, endVector);
			nDotL = 1.0;

			float widthSquared = l.directionWidth.w * l.directionWidth.w;
			float minDist = min(startDist, endDist);
			minDist = max(startDist, endDist) < widthSquared ? dist : minDist;
			attenuation = minDist * minDist;
		}
		
		attenuation = l.color.w / attenuation;

		float3 halfway = normalize(normalize(lightVector) + cameraVector);
		float nDotH = saturate(dot(normal, halfway));
		float glossTerm = ((gloss + 2) / 8) * pow(nDotH, gloss);
		float3 fresnelTerm = (specularColor) + (1.0f - specularColor) * pow(1.0f - nDotH, 5);
		
		diffuseLight.rgb += max(l.color.rgb * (float3(1.0, 1.0, 1.0) - fresnelTerm) * nDotL * attenuation * directional * shadowSpotVisible, 0.0);

		specularLight.rgb += glossTerm * fresnelTerm * l.color.rgb * nDotL * attenuation * directional * shadowSpotVisible;
	}

	float4 outColor = (texColor + color) * (diffuseLight + ambient);
	outColor += specularLight;

	output.color = outColor;
	output.color.a = texColor.a + color.a;
	output.depth = float4(input.Pos.z, 0.0, 0.0, 1.0);


	return output;
}
