#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
{
	this->particles = new Particle[capParticle];
}

ParticleSystem::~ParticleSystem()
{
}

bool ParticleSystem::addParticle(int nrOf, int lifeTime, Vector3 position, Vector3 initialDirection)
{
	return false;
}
