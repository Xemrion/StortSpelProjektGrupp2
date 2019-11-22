cbuffer CB_PER_FRAME : register(b0)
{
	float4x4 viewProj;
	float4 camPos;
	float4 upp;//.w is 0 or 1
}

cbuffer ParticleRenderParams : register(b1)
{
	float4 colors[4];
	float4 config; //.x = nrOfColors, .y = startSize .z = endSize
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
	float4 pos : POSITION;
	float2 time : TIME;
	uint ind : VAR;
};
struct GSOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
	float2 uv : UV;
};
float4 fadeOverTime(float4 startColor, float4 endColor, float time, float totTime)
{
	float4 fadedColor;
	fadedColor.x = lerp(startColor.x, endColor.x, time / totTime);
	fadedColor.y = lerp(startColor.y, endColor.y, time / totTime);
	fadedColor.z = lerp(startColor.z, endColor.z, time / totTime);
	fadedColor.w = lerp(startColor.w, endColor.w, time / totTime);
	return fadedColor;

};
[maxvertexcount(3)]
void main(point GSInput input[1], inout TriangleStream<GSOutput> theOutput)
{
	GSOutput output;

	float3 toCamera =  normalize(camPos.xyz - input[0].pos.xyz);

	float3 right = cross(toCamera, upp.xyz);
	
	float3 up = normalize(cross(right, toCamera));
	float3 vert[4];
	float time = input[0].time.x;
	float totalLifeTime = input[0].time.y;
	float size;

	/*
	Config.x is nrOfColors. Max 4
	Config.y is the startsize and config.z is the end size.
	*/
	float startSize = config.y;
	float endSize = config.z;
	size = lerp(startSize, endSize, smoothstep(0.0, totalLifeTime - 1.0f, time)) * (1.0 - smoothstep(totalLifeTime - 1.0f, totalLifeTime, time));

	
	float nrOfColors = config.x;
	float4 testColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
	float halfTime = totalLifeTime / (nrOfColors-1);
	if (time < halfTime)
	{
		testColor = fadeOverTime(colors[0], colors[1], time, halfTime);
	}
	else if (time < halfTime * 2)
	{
		testColor = fadeOverTime(colors[1], colors[2], time % halfTime, halfTime);
	}
	else if(time  < halfTime * 3)
	{
		testColor = fadeOverTime(colors[2], colors[3], time % halfTime, halfTime);
	}
	
	//vert[0] = input[0].pos - right *size + up * size; // Top middle
	//vert[1] = input[0].pos + right *size + up * size; // Top right
	//vert[3] = input[0].pos + right *size - up * size; // Bottom right
	//vert[2] = input[0].pos - right *size - up * size; // Top right 
	float3 testUp = up;//float3(0, 0, 1);
	float3 testRight = right;//float3(1, 0, 0);
	//Change every frame between up-pointy triangle and down-pointy triangle
	if (upp.w>0.9f)
	{
		vert[0] = input[0].pos.xyz + testUp * size; // Top middle
		vert[1] = input[0].pos.xyz + testRight * size - testUp * size; // Top right
		vert[2] = input[0].pos.xyz - testRight * size - testUp * size; // Top right 
	}						  
	else					  
	{						 
		vert[0] = input[0].pos.xyz - testRight * size + testUp * 0.50f * size; // Top middle
		vert[1] = input[0].pos.xyz + testRight * size + testUp * size*0.50f; // Top right
		vert[2] = input[0].pos.xyz - testUp * 1.50f *size; // Top right 
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
		output.color = testColor;
		theOutput.Append(output);
	}
}