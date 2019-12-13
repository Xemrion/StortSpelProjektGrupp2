struct Particle
{
	float4 position;//.w = initialSize
	float4 direction;//.w=size
	float2 time;
};

StructuredBuffer<Particle> SimulationState;

struct VS_INPUT
{
	uint vertexId  : SV_VertexID;
    uint instanceId : SV_InstanceID;
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
	
    output.pos.xyz = SimulationState[input.vertexId].position.xyz;
    output.pos.w = SimulationState[input.vertexId].direction.w;
    output.ind = input.vertexId;
    output.time = SimulationState[input.vertexId].time;
    output.direction = SimulationState[input.vertexId].direction;
	return output;
}