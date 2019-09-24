struct Particle
{
	float3 position;
	float3 direction;
	float time;
};

StructuredBuffer<Particle> SimulationState;

struct VS_INPUT
{
	uint vertexid : SV_VertexID;
};

struct VS_OUT
{
	float3 pos : POSITION;
};

VS_OUT main(in VS_INPUT input)
{
	VS_OUT output = (VS_OUT)0;
	
	output.pos.xyz = SimulationState[input.vertexid].position;

	return output;
}