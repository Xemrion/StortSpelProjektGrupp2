struct Particle
{
	float4 position;//.w = initialSize
	float4 direction;//.w=size
	float2 time;
};

StructuredBuffer<Particle> SimulationState;

struct VS_INPUT
{
    uint instanceId : SV_InstanceID;
    uint vertexId : SV_VertexID;
};

struct VS_OUT
{
	float4 pos : POSITION;
	float4 direction : DIRECTION;
	float2 time : TIME;
	uint ind : VAR;
};

VS_OUT main(in VS_INPUT input)
{
	VS_OUT output = (VS_OUT)0;
	
    output.pos.xyz = SimulationState[0].position.xyz;
    output.pos.w = SimulationState[0].direction.w;
    output.ind = 0;
    output.time = SimulationState[0].time;
    output.direction = SimulationState[0].direction;
	return output;
}