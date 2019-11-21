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
Texture2DMS<float, 4> depthBuffer : register(t7);
SamplerState SampSt : register(s0);
SamplerState ShadowSamp : register(s1);
StructuredBuffer<TileData> tileData : register(t2);

float4 main(VS_OUT input) : SV_Target
{
	float depthValue = depthBuffer.sample[0][input.Pos.xy];
	depthValue = min(depthValue, depthBuffer.sample[1][input.Pos.xy]);
	depthValue = min(depthValue, depthBuffer.sample[2][input.Pos.xy]);
	depthValue = min(depthValue, depthBuffer.sample[3][input.Pos.xy]);
	
	if (depthValue < input.Pos.z)
	{
		return float4(0.0, 0.0, 0.0, 0.0);
	}
	
	float fogGradient = saturate(((depthValue * input.Pos.w) - (input.Pos.z * input.Pos.w)) * 10);

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
	float4 ambient = max(-dot(sunDir, normal), float4(0.2f, 0.2f, 0.2f, 1.0)) * sunColor;

	float4 diffuseLight = float4(0.0, 0.0, 0.0, 1.0);

	for (int i = 0; i < lightTileData.numLights; ++i)
	{
		Light l = lights[lightTileData.indices[i]];
		float3 lightVector = l.pos.xyz - input.wPos.xyz;
		float attenuation = 0.0;

		float nDotL = 0.0;
		float directional = 1.0;

		//if the light is a spot light
		if (l.pos.w == 1.0)
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
		//if the light is a laser
		if (l.pos.w == 2.0)
		{
			float3 rayPointVector = (l.pos.xyz - input.wPos.xyz) - dot(l.pos.xyz - input.wPos.xyz, l.directionWidth.xyz) * l.directionWidth.xyz;
			float dist = dot(rayPointVector, rayPointVector);

			float startDist = dot(lightVector, lightVector);
			float3 endVector = (l.pos.xyz + l.directionWidth.xyz * l.directionWidth.w) - input.wPos.xyz;
			float endDist = dot(endVector, endVector);

			if (startDist < l.directionWidth.w * l.directionWidth.w && endDist < l.directionWidth.w * l.directionWidth.w)
			{
				attenuation = l.color.w / (dist * dist);
				nDotL = abs(dot(normal, normalize(rayPointVector)));
			}
			else
			{
				attenuation = l.color.w / (min(startDist, endDist)* min(startDist, endDist));
				if (startDist < endDist)
				{
					nDotL = abs(dot(normal, normalize(lightVector)));
				}
				else {
					nDotL = abs(dot(normal, normalize(endVector.xyz)));
				}
			}
		}
		else
		{
			attenuation = l.color.w / dot(lightVector, lightVector);
			nDotL = abs(dot(normal, normalize(lightVector)));
		}
		
		diffuseLight.rgb += max(l.color.rgb * nDotL * attenuation * directional, 0.0) / (3.1416*2.0);
	}

	float4 outColor = (texColor + color) * (diffuseLight + ambient);

	outColor.a *= fogGradient;
	return outColor;
}
