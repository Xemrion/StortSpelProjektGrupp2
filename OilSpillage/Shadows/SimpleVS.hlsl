cbuffer PerFrameMatrices : register(b0)
{
	float4x4 viewProj;
};

cbuffer WorldMatrix : register(b1)
{
	float4x4 worldMatrix;
}
struct VS_IN
{
	float3 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output;

	output.Pos = mul(float4(input.Pos,1.0f), worldMatrix);
	output.Pos = mul(output.Pos, viewProj);
	output.Normal = input.Normal;
	output.Tex = input.Tex;

	return output;
}