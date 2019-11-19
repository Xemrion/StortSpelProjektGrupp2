#ifndef PARTICLEHANDLER_H
#define PARTICLEHANDLER_H
#include"ParticleSystem.h"
class ParticleHandler
{
public:
	ParticleHandler();
	~ParticleHandler();
	//if no name is given the particlesystems name will be used. If given a name the name of the particlesystem will be replaced
	void addParticleSystem(ParticleSystem* system, std::string name = "NONE");
	void addParticleSystem(std::string name, std::string csUpdate = "ParticleUpdateCS.cso", std::string csCreate = "ParticleUpdateCS.cso", std::string gs = "ParticleGS.cso");
	void addParticleSystem(std::string name, Vector4 colors[4], int nrOfColors, float startSize, float endSize, float vectorFieldPower = 0.0f, float vectorFieldSize = 0.0f,
		std::string csUpdate = "ParticleUpdateCS.cso", std::string csCreate = "ParticleUpdateCS.cso", std::string gs = "ParticleGS.cso");

	ParticleSystem* getParticleSystem(std::string name)const;

	bool loadParticleSystem(std::string name);
	bool loadParticleSystems();//load particleSystems
	bool saveParticleSystem(std::string name);
	bool saveParticleSystems();//save particleSystems
private:
	std::unordered_map<std::string, ParticleSystem*> particleSystems;
	

};

#endif // !

