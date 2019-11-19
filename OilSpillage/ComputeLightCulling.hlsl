#include "ShaderDefines.hlsli"

struct Light
{
	float4 pos;
	float4 color;
	float4 directionWidth;
};

struct Frustum {
	float4 left;
	float4 right;
	float4 top;
	float4 bottom;
};

struct TileData
{
	uint numLights;
	uint indices[MAX_LIGHTS_PER_TILE];
};

cbuffer Lights : register(b0)
{
	Light lights[MAX_LIGHTS_TOTAL];
}

cbuffer View : register(b1)
{
	float4x4 view;
}

RWStructuredBuffer<TileData> outLightBuffer : register(u0);
StructuredBuffer<Frustum> frustumInfo : register(t0);

groupshared uint lightCount;
groupshared TileData groupTileData;
groupshared Frustum groupFrustum;

inline bool testPlanePoint(float3 lightPos, float4 plane, float radius)
{
	float dist = dot(lightPos, plane.xyz);
	return dist < radius;
}

inline bool distancePlanePoint(float3 lightPos, float4 plane)
{
	return dot(lightPos, plane.xyz);
}

struct Ray
{
	float3 origin;
	float3 dir;
	float length;
};

inline float distancePlaneRay(Ray ray, float4 plane)
{
	float d = dot(ray.dir, plane.xyz);
	return dot(ray.origin, plane.xyz) / max(d, 0.000001 * sign(d));
}

inline bool testPlaneRay(Ray ray, float4 plane, float radius)
{
	float d0 = dot(ray.origin, plane.xyz);
	float d1 = dot(ray.origin + ray.dir * ray.length, plane.xyz);

	float minDist = min(d0, d1);
	float maxDist = max(d0, d1);

	return minDist <= radius && maxDist >= -radius;
}

void appendLight(uint lightIndex)
{
	uint index;
	InterlockedAdd(lightCount, 1, index);
	if (index < MAX_LIGHTS_PER_TILE)
	{
		InterlockedAdd(groupTileData.numLights, 1);
		groupTileData.indices[index] = lightIndex;
	}
}

[numthreads(LIGHT_CULLING_BLOCK_SIZE, LIGHT_CULLING_BLOCK_SIZE, 1)]
void main(uint groupIndex : SV_GroupIndex, uint3 groupID : SV_GroupID)
{
	uint tileIndex = groupIndex;

	if (groupIndex == 0)
	{
		lightCount = 0;
		groupTileData.numLights = 0;
		groupFrustum = frustumInfo[groupID.y * 80 + groupID.x];
	}

	GroupMemoryBarrierWithGroupSync();

	for (uint i = groupIndex; i < MAX_LIGHTS_TOTAL; i += LIGHT_CULLING_BLOCK_SIZE * LIGHT_CULLING_BLOCK_SIZE)
	{
		bool inside;
		Light l = lights[i];
		if (l.color.w == 0.0f)
		{
			continue;
		}

		float4 pos = float4(l.pos.xyz, 1.0);
		pos.y = -pos.y;
		pos = mul(pos, view);
		float radius = sqrt(l.color.w / 0.05);

		if (l.pos.w == 1.0)
		{
			l.directionWidth = mul(float4(l.directionWidth.xyz, 0.0), view);
			radius *= 2.0;
			pos.xyz += l.directionWidth.xyz * radius;
		}
		if (l.pos.w == 2.0)
		{
			Ray ray;
			ray.origin = pos.xyz;
			ray.dir = normalize(mul(float4(l.directionWidth.xyz, 0.0), view));
			ray.length = l.directionWidth.w;
		
			//inside = testPlaneRay(ray, groupFrustum.left);
			//if (!inside) inside = testPlaneRay(ray, groupFrustum.right);
			//if (!inside) inside = testPlaneRay(ray, groupFrustum.bottom);
			//if (!inside) inside = testPlaneRay(ray, groupFrustum.top);
			//float3 endPos = ray.origin + ray.dir * ray.length;
			
			//float d0 = dot(ray.origin, groupFrustum.left);
			//float d1 = dot(ray.origin + ray.dir * ray.length, groupFrustum.left);
			//float minDist = min(d0, d1);
			//float maxDist = max(d0, d1);
			//inside = ((minDist < 0.0) && (maxDist > 0.0));
			inside = false;
			float radius = sqrt(sqrt(l.color.w / 0.05));
			int intersections = 0;
			if (testPlaneRay(ray, groupFrustum.left, radius)) intersections++;
			if (testPlaneRay(ray, groupFrustum.right, radius)) intersections++;
			if (testPlaneRay(ray, groupFrustum.top, radius)) intersections++;
			if (testPlaneRay(ray, groupFrustum.bottom, radius)) intersections++;

			if (intersections == 4) inside = true;
			//inside = !(distancePlanePoint(pos.xyz, groupFrustum.left, radius) && testPlanePoint(endPos, groupFrustum.left, radius));
			//if (inside) inside = !(testPlanePoint(pos.xyz, groupFrustum.right, radius) && testPlanePoint(endPos, groupFrustum.right, radius));
			//if (inside) inside = !(testPlanePoint(pos.xyz, groupFrustum.top, radius) && testPlanePoint(endPos, groupFrustum.top, radius));
			//if (inside) inside = !(testPlanePoint(pos.xyz, groupFrustum.bottom, radius) && testPlanePoint(endPos, groupFrustum.bottom, radius));
		}
		else
		{
			inside = testPlanePoint(pos.xyz, groupFrustum.left, radius);
			if (inside) inside = testPlanePoint(pos.xyz, groupFrustum.right, radius);
			if (inside) inside = testPlanePoint(pos.xyz, groupFrustum.top, radius);
			if (inside) inside = testPlanePoint(pos.xyz, groupFrustum.bottom, radius);
			//inside = false;
		}

		if (inside)
		{
			appendLight(i);
		}
	}

	GroupMemoryBarrierWithGroupSync();

	if (groupIndex == 0)
	{
		outLightBuffer[groupID.y * 80 + groupID.x] = groupTileData;
	}
}
