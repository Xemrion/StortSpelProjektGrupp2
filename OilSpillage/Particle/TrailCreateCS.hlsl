struct Particle
{
	float4 position;
	float4 direction;
	float2 time;//in trail time.y = alive > 0
};

globallycoherent RWStructuredBuffer<Particle> NewSimulationState : register(u0);
globallycoherent RWStructuredBuffer<Particle> DeadList : register(u1);


cbuffer ParticleParameters : register(b0)
{
    float4 emitterLocation; //w = totalTime
    float4 randomVector;
    float4 initialDirection; // trailDirection (opposite of carDir)
};


[numthreads(4, 1, 1)]
void main(uint3 DispatchThreadID : SV_DispatchThreadID)
{
	Particle p;
	float frameID = initialDirection.w;
    p.position.xyz = emitterLocation.xyz + initialDirection.xyz * DispatchThreadID.x * 0.1f;
	p.position.w = 1.0f;
	p.direction = initialDirection;
	p.time.x = 0.0f;
    p.time.y = 2.0f;
    //means the particle will be overwritten and should be sent to the deadlist
    if(NewSimulationState[DispatchThreadID.x + int(randomVector.x)].time.y > 0.0f)
    {
        DeadList[DeadList.IncrementCounter()] = NewSimulationState[DispatchThreadID.x + int(randomVector.x)];
    }

    NewSimulationState[DispatchThreadID.x + int(randomVector.x)] = p;
    NewSimulationState.IncrementCounter();
    
   

}