struct PSInput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : UV;
};
Texture2D Texture : register(t0);
SamplerState Sampler : register(s0);


float4 main(PSInput input) : SV_Target
{
    return input.color*2;
}