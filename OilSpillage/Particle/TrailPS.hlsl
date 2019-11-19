struct PSInput
{
	float4 pos : SV_POSITION;
	float4 shadowPos : SHADOWPOS;
	float4 color : COLOR;
	float2 uv : UV;
};
Texture2D Texture : register(t0);
Texture2D ShadowMap : register(t1);
SamplerState Sampler: register(s0);
float4 main(PSInput input) : SV_Target
{
	//float4 color = Texture.Sample(Sampler,input.uv);
	return input.color;
}