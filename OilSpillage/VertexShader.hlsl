struct VS_IN
{
	float3 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
	uint InstanceID : SV_InstanceID;
};

struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float4 wPos: APOS;
	float2 Tex : TEXCOORD;
	float4 Normal : NORMAL;
};

cbuffer CB_PER_FRAME : register(b0)
{
	float4x4 viewProj;
}

cbuffer world : register(b1)
{
	float4x4 world[64];
}
//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;
	output.Pos = mul(float4(input.Pos, 1.0f), world[input.InstanceID]);
	output.wPos = output.Pos;
	output.Pos = mul(output.Pos, viewProj);
	output.Tex = input.Tex;
	output.Normal = mul(float4(input.Normal, 0.0f), world[input.InstanceID]);
	output.Normal.xyz = normalize(output.Normal.xyz);
	
	return output;
}