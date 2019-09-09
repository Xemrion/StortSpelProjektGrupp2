//#pragma pack_matrix( row_major )

cbuffer CB_PER_FRAME : register(b0)
{
	float4x4 viewProj;//view*projection
}

cbuffer world : register(b1)
{
	float4x4 world;//world
}

struct VS_INPUT
{
	float3 inPos : POSITION;
	float3 inColor : COLOR;
};

struct VS_OUTPUT
{
	float4 outPosition : SV_POSITION;
	float3 outColor : COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT output;
	output.outPosition = mul(float4(input.inPos, 1.0f), world);
	output.outColor = input.inColor;
	return output;
}