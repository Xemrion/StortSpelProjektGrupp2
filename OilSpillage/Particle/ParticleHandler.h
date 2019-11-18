#ifndef PARTICLEHANDLER_H
#define PARTICLEHANDLER_H
#include"ParticleSystem.h"
class ParticleHandler
{
public:
	ParticleHandler();
	~ParticleHandler();

	

	void addParticleSystem(ParticleSystem* system);
	void addParticleSystem(const wchar_t* csUpdate, const wchar_t* csCreate = nullptr, const wchar_t* gs = nullptr);
	void addParticleSystem(Vector4 colors[4], int nrOfColors, float startSize, float endSize, float vectorFieldPower = 0.0f, float vectorFieldSize = 0.0f,
		const wchar_t* csUpdate = nullptr, const wchar_t* csCreate = nullptr, const wchar_t* gs = nullptr);

	ParticleSystem* getParticleSystem(std::string name)const;

	bool loadParticleSystems(std::string filePath);//load particleSystems from file

private:
	std::unordered_map<std::string, ParticleSystem*> particleSystems;
	

};

#endif // !

