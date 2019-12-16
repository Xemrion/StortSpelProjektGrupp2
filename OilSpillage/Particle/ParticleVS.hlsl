struct Particle
{
	float4 position;//.w = initialSize
	float4 direction;//.w=size
	float2 time;
};
StructuredBuffer<Particle> SimulationState;
cbuffer CB_PER_FRAME : register(b0)
{
    float4x4 viewProj;
    float4 camPos;
    float4 upp; //.w is 0 or 1
}

cbuffer ParticleRenderParams : register(b1)
{
    float4 colors[4];
    float4 config; //.x = nrOfColors, .y = startSize .z = endSize
};

struct VS_INPUT
{
    uint instanceId : SV_InstanceID;
    uint vertexId : SV_VertexID;
};

struct VS_OUT
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
VS_OUT main(in VS_INPUT input)
{
	VS_OUT output = (VS_OUT)0;
	
    float3 vert;
    
    float3 toCamera = normalize(camPos.xyz - SimulationState[input.instanceId].position.xyz);
    float3 right = cross(toCamera, upp.xyz);
    float3 up = normalize(cross(right, toCamera));
    
    float time = SimulationState[input.instanceId].time.x;
    float totalLifeTime = SimulationState[input.instanceId].time.y;
    
    float startSize = config.y;
    float endSize = config.z;
    float size = 0.0f;
    size = lerp(startSize, endSize, smoothstep(0.0, totalLifeTime - 1.0f, time)) * (1.0 - smoothstep(totalLifeTime - 1.0f, totalLifeTime, time));
    
    float nrOfColors = config.x;
    float4 testColor = float4(1.0f, 0.0f, 1.0f, 1.0f);
    float halfTime = totalLifeTime / (nrOfColors - 1);
    if (time < halfTime)
    {
        testColor = fadeOverTime(colors[0], colors[1], time, halfTime);
    }
    else if (time < halfTime * 2)
    {
        testColor = fadeOverTime(colors[1], colors[2], time % halfTime, halfTime);
    }
    else if (time < halfTime * 3)
    {
        testColor = fadeOverTime(colors[2], colors[3], time % halfTime, halfTime);
    }
    float2 texCoord;
    float3 testUp = up;
    float3 testRight = right;
    if(upp.w > 0.9f)
    {
        if(input.vertexId == 0)
        {
            vert = SimulationState[input.instanceId].position.xyz + testUp * size; // Top middle
            texCoord = float2(0.5f, 1.0f);
        }
        else if (input.vertexId == 1)
        {
            vert = SimulationState[input.instanceId].position.xyz + testRight * size - testUp * size; // Top right
            texCoord = float2(1.0f, 0.0f);

        }
        else if (input.vertexId == 2)
        {
            vert = SimulationState[input.instanceId].position.xyz - testRight * size - testUp * size; // Top right 
            texCoord = float2(0.0f, 0.0f);

        }
    }
    else
    {
        if (input.vertexId == 0)
        {
            vert = SimulationState[input.instanceId].position.xyz - testRight * size + testUp * 0.50f * size; // Top middle
            texCoord = float2(0.0f, 1.0f);

        }
        else if (input.vertexId == 1)
        {
            vert = SimulationState[input.instanceId].position.xyz + testRight * size + testUp * size * 0.50f; // Top right
            texCoord = float2(1.0f, 1.0f);

        }
        else if (input.vertexId == 2)
        {
            vert = SimulationState[input.instanceId].position.xyz - testUp * 1.50f * size; // Top right 
            texCoord = float2(0.5f, 0.0f);

        }
    }
    
	
   

    output.pos = mul(float4(vert, 1.0f), viewProj);
    output.uv = texCoord;
    output.color = testColor;
    
    
	return output;
}