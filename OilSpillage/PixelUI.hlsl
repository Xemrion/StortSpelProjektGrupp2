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

cbuffer MaterialBuffer : register(b0)
{
	float4 color;
}

cbuffer SunInfo : register(b2) 
{
	float4 sunColor;
	float4 sunDir;
};

Texture2D Tex : register(t0);
Texture2D NormalMap : register(t1);
Texture2D SpecularMap : register(t5);
Texture2D GlossMap : register(t6);
SamplerState SampSt : register(s0);
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

	float4 ambient = max(-dot(sunDir, normal), float4(0.2f, 0.2f, 0.2f, 1.0)) * sunColor;

	float4 diffuseLight = float4(0.0, 0.0, 0.0, 1.0);
	float4 specularLight = float4(0.0, 0.0, 0.0, 0.0);

	//for (int i = 0; i < lightTileData.numLights; ++i)
	//{
	//	Light l = lights[lightTileData.indices[i]];
	//	float3 lightVector = l.pos.xyz - input.wPos.xyz;
	//	float attenuation = l.color.w / dot(lightVector, lightVector);
	//	if (attenuation < 0.0005) attenuation = 0;
	//	float nDotL = max(dot(normal, normalize(lightVector)), 0.0);
	//	float directional = 1.0;
	//	shadowSpotVisible = 1.0f;
	//	//if the light is a spot light
	//	if (l.directionWidth.w > 0.0)
	//	{
	//		directional = 0.0;
	//		float s = dot(-normalize(lightVector), l.directionWidth.xyz);
	//		float umbra = cos(l.directionWidth.w);
	//		if (s > umbra) {
	//			float penumbra = cos(l.directionWidth.w * 0.9);
	//			directional = (s - umbra) / (penumbra - umbra);
	//			directional *= directional;
	//		}
	//		if (lightTileData.indices[i] == indexForSpot)
	//		{
	//			shadowSpotVisible = shadowVisible(input.shadowPosSpot, ShadowMapSpot, 0.005f);
	//			shadowSpotVisible = 1 - shadowSpotVisible;
	//		}
	//	}

	//	diffuseLight.rgb += max(l.color.rgb * nDotL * attenuation * directional * shadowSpotVisible, 0.0);
	//	float3 cameraVector = normalize(cameraPos.xyz - input.wPos.xyz);
	//	float3 halfway = normalize(normalize(lightVector) + cameraVector);
	//	specularLight.rgb += l.color.rgb * pow(max(dot(normal, halfway), 0.0), gloss) * attenuation * directional * shadowSpotVisible;
	//}

	float4 outColor = (texColor + color) * (diffuseLight + ambient);
	outColor += (specularColor + color) * specularLight;

	return outColor;
}
