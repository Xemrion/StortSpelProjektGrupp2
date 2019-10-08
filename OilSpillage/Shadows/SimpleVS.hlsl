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
	float3 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
};

VS_OUT main(VS_IN input)
{
	VS_OUT output;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewProj);
	output.Normal = input.Normal;
	output.Tex = input.Tex;

	return output;
}