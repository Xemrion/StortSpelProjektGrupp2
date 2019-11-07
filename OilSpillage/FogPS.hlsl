struct VS_OUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	float3 Normal : NORMAL;
};

struct PS_OUT
{
	float4 color;
	float4 normal;
};

cbuffer MaterialBuffer : register(b0)
{
	float3 color;
	float densityThreshold;
	float density;
	float ambientDensity;
	float normalY;
}

Texture2D noiseTex : register(t0);
SamplerState samplerState : register(s0);


float3 hash(float3 p) // replace this by something better. really. do
{
	p = float3(dot(p, float3(127.1, 311.7, 74.7)),
		dot(p, float3(269.5, 183.3, 246.1)),
		dot(p, float3(113.5, 271.9, 124.6)));

	return -1.0 + 2.0 * frac(sin(p) * 43758.5453123);
}

// return value noise (in x) and its derivatives (in yzw)
float4 noised(in float3 x)
{
	// grid
	float3 i = floor(x);
	float3 w = frac(x);

#if 1
	// quintic interpolant
	float3 u = w * w * w * (w * (w * 6.0 - 15.0) + 10.0);
	float3 du = 30.0 * w * w * (w * (w - 2.0) + 1.0);
#else
	// cubic interpolant
	float3 u = w * w * (3.0 - 2.0 * w);
	float3 du = 6.0 * w * (1.0 - w);
#endif    

	// gradients
	float3 ga = hash(i + float3(0.0, 0.0, 0.0));
	float3 gb = hash(i + float3(1.0, 0.0, 0.0));
	float3 gc = hash(i + float3(0.0, 1.0, 0.0));
	float3 gd = hash(i + float3(1.0, 1.0, 0.0));
	float3 ge = hash(i + float3(0.0, 0.0, 1.0));
	float3 gf = hash(i + float3(1.0, 0.0, 1.0));
	float3 gg = hash(i + float3(0.0, 1.0, 1.0));
	float3 gh = hash(i + float3(1.0, 1.0, 1.0));

	// projections
	float va = dot(ga, w - float3(0.0, 0.0, 0.0));
	float vb = dot(gb, w - float3(1.0, 0.0, 0.0));
	float vc = dot(gc, w - float3(0.0, 1.0, 0.0));
	float vd = dot(gd, w - float3(1.0, 1.0, 0.0));
	float ve = dot(ge, w - float3(0.0, 0.0, 1.0));
	float vf = dot(gf, w - float3(1.0, 0.0, 1.0));
	float vg = dot(gg, w - float3(0.0, 1.0, 1.0));
	float vh = dot(gh, w - float3(1.0, 1.0, 1.0));

	// interpolations
	return float4(va + u.x * (vb - va) + u.y * (vc - va) + u.z * (ve - va) + u.x * u.y * (va - vb - vc + vd) + u.y * u.z * (va - vc - ve + vg) + u.z * u.x * (va - vb - ve + vf) + (-va + vb + vc - vd + ve - vf - vg + vh) * u.x * u.y * u.z,    // value
		ga + u.x * (gb - ga) + u.y * (gc - ga) + u.z * (ge - ga) + u.x * u.y * (ga - gb - gc + gd) + u.y * u.z * (ga - gc - ge + gg) + u.z * u.x * (ga - gb - ge + gf) + (-ga + gb + gc - gd + ge - gf - gg + gh) * u.x * u.y * u.z +   // derivatives
		du * (float3(vb, vc, ve) - va + u.yzx * float3(va - vb - vc + vd, va - vc - ve + vg, va - vb - ve + vf) + u.zxy * float3(va - vb - ve + vf, va - vb - vc + vd, va - vc - ve + vg) + u.yzx * u.zxy * (-va + vb + vc - vd + ve - vf - vg + vh)));
}


PS_OUT main(VS_OUT input) : SV_TARGET
{
	//float alpha = noiseTex.Sample(samplerState, input.Pos.xy / 500).r < densityThreshold ? 0.0 : density;
	//int2 offset = int2(1, 0);
	//float l = noiseTex.Sample(samplerState, input.Pos.xy / 500, -offset.xy).r;
	//float r = noiseTex.Sample(samplerState, input.Pos.xy / 500, offset.xy).r;
	//float u = noiseTex.Sample(samplerState, input.Pos.xy / 500, -offset.yx).r;
	//float d = noiseTex.Sample(samplerState, input.Pos.xy / 500, offset.yx).r;
	
	float4 nd = noised(float3(input.Pos.xy * 0.1, normalY));
	float alpha = nd.x < densityThreshold ? density : 0.0;
	//float alpha = 0.0;
	//int radius = 1;
	//for (int i = -radius; i <= radius; ++i)
	//{
	//	for (int j = -radius; j <= radius; ++j)
	//	{
	//		for (int k = -radius; k <= radius; ++k)
	//		{
	//			float3 pos = (input.Pos.xy * 0.1, normalY) + float3(i, j, k);
	//			float factor = (1 / (2 * 3.1416 * radius * radius * radius)) * pow(2.718, -(dot(pos, pos) / (2 * radius * radius)));
	//			alpha += noised(pos).x < densityThreshold ? density * factor : 0.0;
	//		}
	//	}
	//}

	PS_OUT output;
	output.color = float4(color.r, color.g, color.b, alpha + ambientDensity);
	//output.normal = lerp(float4(normalize(float3(r - l, d - u, normalY)) * 0.5 + 0.5, 1.0), float4(float3(0.0, 0.0, 1.0) * 0.5 + 0.5, 1.0), ambientDensity / (alpha + ambientDensity));
	//output.normal = float4(normalize(float3(r - l, d - u, normalY)) * 0.5 + 0.5, 1.0);
	nd = noised(float3(input.Pos.xy * 0.1, normalY));
	output.normal.xyz = nd.yzw * 0.5 + 0.5;

	return output;
}