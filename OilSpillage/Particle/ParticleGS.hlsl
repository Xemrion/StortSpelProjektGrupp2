struct GSInput
{
	float4 Pos : SV_POSITION;
	float4 color: COLOR;
};

struct GSOutput
{
	float4 Pos: SV_POSITION;
	float4 color: COLOR;
	float2 uv : TEXCOORD;
};

cbuffer CB_PER_FRAME : register(b0)
{
	float4x4 viewProj;
	float4 camPos;
	float3 upp;
}
[maxvertexcount(4)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> theOutput)
{
	float3 planeNormal = input[0].Pos - camPos.xyz;
	//planeNormal.y = 0.0f;
	planeNormal = normalize(planeNormal);
	float3 right = normalize(cross(planeNormal, upp));

	float3 up = /*normalize(upp);*/normalize(cross(planeNormal, right));
	float3 vert[4];
	//vert[0] = input[0].Pos - right *1 - up * 1; // Bottom left
	//vert[1] = input[0].Pos + right *1 - up * 1; // Bottom right
	//vert[2] = input[0].Pos - right *1 + up * 1; // Top left
	//vert[3] = input[0].Pos + right *1 + up * 1; // Top right 

	vert[0] = input[0].Pos - right * input[0].Pos.w + up * input[0].Pos.w; // Top middle
	vert[1] = input[0].Pos + right * input[0].Pos.w + up * input[0].Pos.w; // Top right
	vert[3] = input[0].Pos + right * input[0].Pos.w - up * input[0].Pos.w; // Bottom right
	vert[2] = input[0].Pos - right * input[0].Pos.w - up * input[0].Pos.w; // Top right 

	float2 texCoord[4];
	texCoord[2] = float2(0, 1);//2
	texCoord[3] = float2(1, 1);//3
	texCoord[0] = float2(0, 0);//0
	texCoord[1] = float2(1, 0);//1

	GSOutput output;
	for (int i = 0; i < 4; i++)
	{
		output.Pos = mul(float4(vert[i], 1.0f), viewProj);
		output.uv = texCoord[i];
		output.color = input[0].color;
		theOutput.Append(output);
	}
}
