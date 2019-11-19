#include"ParticleHandler.h"

ParticleHandler::ParticleHandler()
{
}

ParticleHandler::~ParticleHandler()
{
	bool result = false;
	for (auto i = this->particleSystems.begin(); i != this->particleSystems.end(); i++)
	{
		delete i->second;
	}
}

void ParticleHandler::addParticleSystem(ParticleSystem* system, std::string name)
{
	if (name == "NONE")
	{
		this->particleSystems[system->getName()] = system;
	}
	else
	{
		system->setNameofSystem(name);
		this->particleSystems[name] = system;
	}
}

void ParticleHandler::addParticleSystem(std::string name, std::string csUpdate, std::string csCreate, std::string gs)
{
	ParticleSystem* newSystem = new ParticleSystem;
	newSystem->setNameofSystem(name);
	newSystem->setParticleShaders(csUpdate, csCreate, gs);
	
	this->particleSystems[newSystem->getName()] = newSystem;
}

void ParticleHandler::addParticleSystem(std::string name, Vector4 colors[4], int nrOfColors, float startSize, float endSize, float vectorFieldPower, float vectorFieldSize, std::string csUpdate, std::string csCreate, std::string gs)
{
	ParticleSystem* newSystem = new ParticleSystem;
	newSystem->setNameofSystem(name);
	newSystem->setParticleShaders(csUpdate, csCreate, gs);
	newSystem->changeColornSize(colors, nrOfColors, startSize, endSize);
	newSystem->changeVectorField(vectorFieldPower, vectorFieldSize);

	this->particleSystems[newSystem->getName()] = newSystem;
}

ParticleSystem* ParticleHandler::getParticleSystem(std::string name) const
{
	return this->particleSystems.at(name) != nullptr ? this->particleSystems.at(name) : nullptr;
}

bool ParticleHandler::loadParticleSystem(std::string name)
{
	return this->particleSystems.at(name)->loadSystem();
}

bool ParticleHandler::loadParticleSystems()
{
	bool result = true;
	for (auto i = this->particleSystems.begin(); i != this->particleSystems.end() && result == true; i++)
	{
		result = i->second->loadSystem();
	}
	return result;
}

bool ParticleHandler::saveParticleSystem(std::string name)
{
	return this->particleSystems.at(name)->saveSystem();
}

bool ParticleHandler::saveParticleSystems()
{
	bool result = true;
	for (auto i = this->particleSystems.begin(); i != this->particleSystems.end() && result == true; i++)
	{
		result = i->second->saveSystem();
	}
	return result;
}
