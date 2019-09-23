struct PSInput
{
	float4 Pos: SV_POSITION;
	float4 color: COLOR;
	float2 uv : TEXCOORD;
};
float4 main(PSInput input) : SV_Target
{
	return input.color;
}