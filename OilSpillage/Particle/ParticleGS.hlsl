cbuffer CB_PER_FRAME : register(b0)
{
	float4x4 viewProj;
	float4 camPos;
	float4 upp;//.w is 0 or 1
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
	float time : TIME;
	float4 color : COLOR;
	uint ind : VAR;
};
struct GSOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : UV;
};
[maxvertexcount(3)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> theOutput)
{
	GSOutput output;

	float dist = saturate(length(input[0].pos - consumerLocation.xyz) / 100.0f);

	float4 color = float4(0.2f, 0.2f, 1.0f, 0.0f) * (dist)
		+float4(1.0f, 0.2f, 0.2f, 0.0f) * (1.0f - dist);

	float3 toCamera =  normalize(camPos.xyz - input[0].pos);
	//planeNormal.y = 0.0f;
	//planeNormal = normalize(planeNormal);
	float3 right = cross(toCamera, upp);
	
	float3 up = /*normalize(upp);*/normalize(cross(right, toCamera));
	float3 vert[4];
	//vert[0] = input[0].Pos - right *1 - up * 1; // Bottom left
	//vert[1] = input[0].Pos + right *1 - up * 1; // Bottom right
	//vert[2] = input[0].Pos - right *1 + up * 1; // Top left
	//vert[3] = input[0].Pos + right *1 + up * 1; // Top right 
	float size = 0.05f;
	
	//size -= input[0].time * 0.01f;
	//float3 color1 =float3(1.0f,0.0f,0.0f);//red
	//float3 color2=float3(0.0f, 0.0f, 1.0f);//blue
	//float3 color3 = float3(0.0f, 1.0f, 0.0f);
	//float3 deltaColor = (color2 - color1) / 10.0f;
	//float3 particleColor = deltaColor*
	//float timeLeft = 10.0f - input[0].time;
	//float3 fadedColor = (color1 * (timeLeft / 10)) + (color2 * (input[0].time / 10)) + (color3 * (timeLeft / 10));
	if (size <= 0)
	{
		size = 0.0f;
	}
	//vert[0] = input[0].pos - right *size + up * size; // Top middle
	//vert[1] = input[0].pos + right *size + up * size; // Top right
	//vert[3] = input[0].pos + right *size - up * size; // Bottom right
	//vert[2] = input[0].pos - right *size - up * size; // Top right 
	if (upp.w>0.9f)
	{
		vert[0] = input[0].pos + up * size; // Top middle
		vert[1] = input[0].pos + right * size - up * size; // Top right
		vert[2] = input[0].pos - right * size - up * size; // Top right 
	}
	else
	{
		vert[0] = input[0].pos - right * size + up * 0.50f * size; // Top middle
		//vert[0].y = vert[0].y * 0.75;
		vert[1] = input[0].pos + right * size + up * size*0.50f; // Top right
		//vert[1].y = vert[1].y * 0.75;
		vert[2] = input[0].pos - up * 1.50f *size; // Top right 
		//vert[2].y = vert[2].y * 1.25;
	}
	
	float2 texCoord[4];
	texCoord[2] = float2(0, 1);//2
	texCoord[3] = float2(1, 1);//3
	texCoord[0] = float2(0, 0);//0
	texCoord[1] = float2(1, 0);//1
	
	for (int i = 0; i < 3; i++)
	{
		output.pos = mul(float4(vert[i], 1.0f), viewProj);
		output.uv = texCoord[i];
		output.color = input[0].color;
		theOutput.Append(output);
	}
}