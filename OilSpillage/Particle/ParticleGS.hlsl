cbuffer CB_PER_FRAME : register(b0)
{
	float4x4 viewProj;
	float4 camPos;
	float3 upp;
}

cbuffer ParticleRenderParams : register(b1)
{
	float4 emitterLocation;
	float4 consumerLocation;
};

static const float4 position[4] =
{
	float4(-1,1,0,0),
	float4(1,1,0,0),
	float4(-1,-1,0,0),
	float4(1,-1,0,0)
};
struct GSInput
{
	float3 pos : POSITION;
};
struct GSOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : UV;
};
[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> theOutput)
{
	GSOutput output;

	float dist = saturate(length(input[0].pos - consumerLocation.xyz) / 100.0f);

	float4 color = float4(0.2f, 0.2f, 1.0f, 0.0f) * (dist)
		+float4(1.0f, 0.2f, 0.2f, 0.0f) * (1.0f - dist);

	float3 planeNormal = input[0].pos - camPos.xyz;
	//planeNormal.y = 0.0f;
	planeNormal = normalize(planeNormal);
	float3 right = normalize(cross(planeNormal, upp));
	
	float3 up = /*normalize(upp);*/normalize(cross(planeNormal, right));
	float3 vert[4];
	//vert[0] = input[0].Pos - right *1 - up * 1; // Bottom left
	//vert[1] = input[0].Pos + right *1 - up * 1; // Bottom right
	//vert[2] = input[0].Pos - right *1 + up * 1; // Top left
	//vert[3] = input[0].Pos + right *1 + up * 1; // Top right 
	
	vert[0] = input[0].pos - right * 0.1 + up * 0.1; // Top middle
	vert[1] = input[0].pos + right * 0.1 + up * 0.1; // Top right
	vert[3] = input[0].pos + right * 0.1 - up * 0.1; // Bottom right
	vert[2] = input[0].pos - right * 0.1 - up * 0.1; // Top right 
	
	float2 texCoord[4];
	texCoord[2] = float2(0, 1);//2
	texCoord[3] = float2(1, 1);//3
	texCoord[0] = float2(0, 0);//0
	texCoord[1] = float2(1, 0);//1
	
	for (int i = 0; i < 4; i++)
	{
		output.pos = mul(float4(vert[i], 1.0f), viewProj);
		output.uv = texCoord[i];
		output.color = color;
		theOutput.Append(output);
	}
}