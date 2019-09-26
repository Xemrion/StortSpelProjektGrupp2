struct Particle
{
	float3 position;
	float3 direction;
	float4 color;
	float time;
};

StructuredBuffer<Particle> SimulationState;

struct VS_INPUT
{
	uint vertexId  : SV_VertexID;
};

struct VS_OUT
{
	float3 pos : POSITION;
	float time : TIME;
	float4 color : COLOR;
	uint ind : VAR;
};

VS_OUT main(in VS_INPUT input)
{
	VS_OUT output = (VS_OUT)0;
	
	output.pos.xyz = SimulationState[input.vertexId].position;
	output.ind = input.vertexId;
	output.time = SimulationState[input.vertexId].time;
	output.color = SimulationState[input.vertexId].color;

	return output;
}