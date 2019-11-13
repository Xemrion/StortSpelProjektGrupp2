#include "ShaderDefines.hlsli"

struct Light
{
	float4 pos;
	float3 color;
	float luminance;
	float3 direction;
	float width;
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

inline bool testPlane(float3 lightPos, float4 plane, float radius)
{
	float dist = dot(lightPos, plane.xyz);
	return dist < radius;
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
void main( uint groupIndex : SV_GroupIndex, uint3 groupID : SV_GroupID )
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
		bool inside = true;
		Light l = lights[i];
		if (l.luminance == 0.0f)
		{
			continue;
		}

		float4 pos = float4(l.pos.xyz, 1.0);
		pos.y = -pos.y;
		pos = mul(pos, view);
		float radius = sqrt(l.luminance / 0.05);

		if (l.width > 0.0)
		{
			l.direction = mul(float4(l.direction, 0.0), view);
			radius *= 2.0;
			pos.xyz += l.direction * radius;
		}
		
		inside = testPlane(pos.xyz, groupFrustum.left, radius);
		if (inside) inside = testPlane(pos.xyz, groupFrustum.right, radius);
		if (inside) inside = testPlane(pos.xyz, groupFrustum.top, radius);
		if (inside) inside = testPlane(pos.xyz, groupFrustum.bottom, radius);

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
