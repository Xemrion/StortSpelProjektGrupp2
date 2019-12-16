#include"ParticleHandler.h"

ParticleHandler::ParticleHandler()
{
}

ParticleHandler::~ParticleHandler()
{
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
	names.push_back(system->getName());
}

void ParticleHandler::addParticleSystem(std::string name, std::string csUpdate, std::string csCreate, std::string vs)
{
	ParticleSystem* newSystem = new ParticleSystem;
	newSystem->setNameofSystem(name);
	newSystem->setParticleShaders(csUpdate, csCreate, vs);
	names.push_back(newSystem->getName());

	this->particleSystems[newSystem->getName()] = newSystem;
}

void ParticleHandler::addParticleSystem(std::string name, Vector4 colors[4], int nrOfColors, float startSize, float endSize, float vectorFieldPower, float vectorFieldSize, std::string csUpdate, std::string csCreate, std::string vs)
{
	ParticleSystem* newSystem = new ParticleSystem;
	newSystem->setNameofSystem(name);
	newSystem->setParticleShaders(csUpdate, csCreate, vs);
	newSystem->changeColornSize(colors, nrOfColors, startSize, endSize);
	newSystem->changeVectorField(vectorFieldPower, vectorFieldSize);
	names.push_back(newSystem->getName());

	this->particleSystems[newSystem->getName()] = newSystem;
}

ParticleSystem* ParticleHandler::getParticleSystem(std::string name) const
{
	ParticleSystem* result = nullptr;
	if (this->particleSystems.find(name) != this->particleSystems.end())
	{
		result = this->particleSystems.at(name);
	}

	return result;
}

const std::vector<std::string>& ParticleHandler::getNames()const
{
	return names;
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

void ParticleHandler::renderParticleSystems(DynamicCamera* camera)
{
	for (auto i = this->particleSystems.begin(); i != this->particleSystems.end(); i++)
	{
		i->second->drawAll(camera);
	}
}

void ParticleHandler::updateParticleSystems(float delta, Matrix viewProj)
{
	for (auto i = this->particleSystems.begin(); i != this->particleSystems.end(); i++)
	{
		i->second->updateParticles(delta, viewProj);
	}
}
