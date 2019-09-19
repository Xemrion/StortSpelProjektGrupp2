#pragma once
#include<d3d11.h>
#include<SimpleMath.h>
using namespace DirectX::SimpleMath;
struct Particle
{
	Vector3 pos, speed;
	Vector4 color;
	float size;
	float angle;
	float weight;
	float life;
};
class ParticleSystem
{
public:
	ParticleSystem();
	~ParticleSystem();
	bool addParticle(int nrOf, int lifeTime, Vector3 initialDirection);

private:
	const int capParticle = 10000;
	int nrOfParticles = 0;
	Particle *particles;


};

