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

struct PointLight
{
	float4 pos;
	float4 color;
};

struct SpotLight
{
	float4 pos;
	float4 color;
	float4 directionWidth;
};

#define MAX_LIGHTS 30
cbuffer Lights : register(b2)
{
	float4 sunVector;
	PointLight pointLights[MAX_LIGHTS];
	SpotLight spotLights[MAX_LIGHTS];
}

Texture2D Tex:register(t0);
SamplerState SampSt :register(s0);

float4 main(VS_OUT input) : SV_Target
{
	float3 normal = input.NormalWS.xyz;
	float4 texColor = Tex.Sample(SampSt, input.Tex).xyzw;

	float ambient = max(dot(input.NormalWS.xyz, sunVector.xyz), 0.2);

	float4 diffuse = float4(0.0, 0.0, 0.0, 1.0);
	for (int i = 0; i < MAX_LIGHTS; ++i) 
	{
		if (pointLights[i].color.w > 0.0)
		{
			float3 lightVector = pointLights[i].pos.xyz - input.wPos.xyz;
			float attenuation = 1.0 / max(dot(lightVector, lightVector), 0.5);
			float nDotL = max(dot(input.NormalWS.xyz, normalize(lightVector)), 0.0);
			diffuse.xyz += max(pointLights[i].color.xyz * nDotL * attenuation * pointLights[i].color.w, 0.0);
		}
	}

	for (int i = 0; i < MAX_LIGHTS; ++i)
	{
		if (spotLights[i].color.w > 0.0)
		{
			float3 lightVector = spotLights[i].pos.xyz - input.wPos.xyz;
			float attenuation = 1.0 / max(dot(lightVector, lightVector), 0.5);
			float nDotL = max(dot(input.NormalWS.xyz, normalize(lightVector)), 0.0);
			float directional = 0.0;
			float s = dot(-normalize(lightVector), spotLights[i].directionWidth.xyz);
			float umbra = cos(spotLights[i].directionWidth.w);
			if (s > umbra) {
				float penumbra = cos(spotLights[i].directionWidth.w * 0.9);
				directional = (s - umbra) / (penumbra - umbra);
				directional *= directional;
				diffuse.xyz += max(spotLights[i].color.xyz * nDotL * attenuation * directional * spotLights[i].color.w, 0.0);
			}
		}
	}

	float4 outColor = (texColor + color) * (diffuse + ambient);
	return outColor / (outColor + float4(1.0, 1.0, 1.0, 0.0));
}
