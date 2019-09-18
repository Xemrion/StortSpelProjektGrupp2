struct VS_OUT
{
	float4 Pos: SV_POSITION;
	float4 wPos: APOS;
	float2 Tex : TEXCOORD;
	float4 NormalWS : NORMAL;
};

struct PointLight
{
	float4 pos;
	float4 color;
};

cbuffer CB_COLOR : register(b0)
{
	float4 color;
}

cbuffer PointLights : register(b2)
{
	PointLight pointLights[20];
}

Texture2D Tex:register(t0);
SamplerState SampSt :register(s0);

float4 main(VS_OUT input) : SV_Target
{
	float3 normal = input.NormalWS.xyz;
	float4 texColor = Tex.Sample(SampSt, input.Tex).xyzw;

	float3 sunVector = normalize(float3(-0.55, 1.0, -0.725));
	float ambient = max(dot(input.NormalWS.xyz, sunVector), float3(0.2, 0.2, 0.2));

	float4 diffuse = float4(0.0, 0.0, 0.0, 1.0);
	for (int i = 0; i < 20; ++i) 
	{
		if (pointLights[i].color.w > 0.0)
		{
			float3 lightVector = pointLights[i].pos.xyz - input.wPos.xyz;
			float distanceFactor = exp(-length(lightVector));
			float nDotL = max(dot(input.NormalWS, normalize(lightVector)), 0.0);
			diffuse.xyz += pointLights[i].color.xyz * nDotL * distanceFactor * pointLights[i].color.w;
		}
		else {
			break;
		}
	}

	float4 outColor = (texColor + color) * (diffuse + ambient);
	return outColor / (outColor + float4(1.0, 1.0, 1.0, 0.0));
}
