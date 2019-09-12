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

Texture2D Tex:register(t0);
SamplerState SampSt :register(s0);
float4 main(VS_OUT input) : SV_Target
{

	float3 normal = input.NormalWS.xyz;
	float4 texColor = Tex.Sample(SampSt, input.Tex).xyzw;
	//float4 texColor = Tex.Sample(SampSt, input.Tex).xyzw;
	return texColor + color;
}