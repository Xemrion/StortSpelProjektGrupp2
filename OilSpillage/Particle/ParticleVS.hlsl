struct Particle
{
	float3 position;
	float4 direction;//.w=size
	float4 color;
	float2 time;
};

StructuredBuffer<Particle> SimulationState;

struct VS_INPUT
{
	uint vertexId  : SV_VertexID;
};

struct VS_OUT
{
	float4 pos : POSITION;
	float time : TIME;
	float4 color : COLOR;
	uint ind : VAR;
};

VS_OUT main(in VS_INPUT input)
{
	VS_OUT output = (VS_OUT)0;
	
	output.pos.xyz = SimulationState[input.vertexId].position;
	output.pos.w = SimulationState[input.vertexId].direction.w;
	output.ind = input.vertexId;
	output.time = SimulationState[input.vertexId].time;
	output.color = SimulationState[input.vertexId].color;


	return output;
}