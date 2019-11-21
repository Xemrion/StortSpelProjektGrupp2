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
SamplerState ShadowSamp : register(s1);

float shadowVisible(float4 shadowPosition, Texture2D shadowMap, float bias)
{
    float4 shadowCoord = shadowPosition;
    shadowCoord.xyz /= shadowCoord.w;
    shadowCoord.xy = shadowCoord.xy * float2(0.5f, -0.5f) + 0.5f;
    float visibility = 0.0f;

    float4 pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(0, 0)).r;
    visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;

    pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(-2, -0)).r;
    visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;

    pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(-2, 0)).r;
    visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;

    pcfDepth = shadowMap.Gather(ShadowSamp, shadowCoord.xy, int2(0, -2)).r;
    visibility += shadowCoord.z - bias > pcfDepth.r ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.g ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.b ? 1.0f : 0.0;
    visibility += shadowCoord.z - bias > pcfDepth.a ? 1.0f : 0.0;
	
    visibility *= 0.25 * 0.25;

    return visibility;
};

float4 main(PSInput input) : SV_Target
{
   // return input.color;
    return (1 - shadowVisible(input.shadowPos, ShadowMap, 0.00015f)) * input.color;
}