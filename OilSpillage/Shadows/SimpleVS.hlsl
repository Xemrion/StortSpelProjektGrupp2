cbuffer PerFrameMatrices : register(b0)
{
	matrix viewMatrix;
	matrix projectionMatrix;
	float3 camera;
};

cbuffer WorldMatrix : register(b1)
{
	matrix worldMatrix;
}
struct VS_IN
{
	float4 position : POSITION;
	float4 color : COLOR;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.color = input.color;

	return output;
}