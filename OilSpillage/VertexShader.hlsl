struct VS_IN
{
	float3 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent : BINORMAL;
	uint InstanceID : SV_InstanceID;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 wPos: APOS;
	float2 Tex : TEXCOORD;
	float4 Normal : NORMAL;
	float4 Tangent : TANGENT;
	float4 Bitangent : BINORMAL;
};

cbuffer CB_PER_FRAME : register(b0)
{
	float4x4 viewProj;
}

cbuffer world : register(b1)
{
	float4x4 world;
}
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.Pos = mul(float4(input.Pos, 1.0f), world);
	output.wPos = output.Pos;
	output.Pos = mul(output.Pos, viewProj);

	output.Tex = input.Tex;

	output.Normal = mul(float4(input.Normal, 0.0f), world);
	output.Normal.xyz = normalize(output.Normal.xyz);

	output.Tangent = mul(float4(input.Tangent, 0.0f), world);
	output.Tangent.xyz = normalize(output.Tangent.xyz);

	output.Bitangent = mul(float4(input.Bitangent, 0.0), world);
	output.Bitangent.xyz = normalize(output.Bitangent.xyz);
	
	return output;
}