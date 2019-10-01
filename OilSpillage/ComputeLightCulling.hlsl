#include "ShaderDefines.hlsli"

struct Light
{
	float4 pos;
	float4 color;
	float4 directionWidth;
};

cbuffer Lights : register(b0)
{
	Light lights[MAX_LIGHTS_TOTAL];
}

cbuffer FrustumPlanes : register(b1)
{
	float4 frustumNear;
	float4 frustumFar;
	float4 frustumLeft;
	float4 frustumRight;
	float4 frustumTop;
	float4 frustumBottom;
}

AppendStructuredBuffer<Light> outLightBuffer : register(u0);

bool testPlane(float3 lightPos, float4 plane, float radius)
{
	float dist = (dot(lightPos, plane.xyz) + plane.w);
	return dist < radius;
}

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
	bool inside = true;
	uint index = DTid.x;
	Light l = lights[index];
	float radius = sqrt(l.color.w * 2.0);
	
	inside = inside && testPlane(l.pos.xyz, frustumLeft, radius);
	inside = inside && testPlane(l.pos.xyz, frustumRight, radius);
	inside = inside && testPlane(l.pos.xyz, frustumTop, radius);
	inside = inside && testPlane(l.pos.xyz, frustumBottom, radius);

	if (inside && l.color.w > 0.0)
	{
		outLightBuffer.Append(l);
	}
}
