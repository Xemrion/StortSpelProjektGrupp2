#ifndef SOUND_H
#define SOUND_H

#include <memory>
#include <unordered_map>
#include <vector>
//#include <d3d11.h>
//#include <SimpleMath.h>
#include <fmod.hpp>

class Sound2
{
private:
	static constexpr const char* SOUND_PATH = "data/sound/";
	static std::unique_ptr<Sound2> instance;

	struct Soundtrack
	{
		FMOD::DSP* filter = nullptr;
		FMOD::Channel* handleCalm = nullptr;
		FMOD::Channel* handleAggressive = nullptr;
		FMOD::ChannelGroup* handleGroup = nullptr;
		float volume = 1.0f;
	} soundtrack;

	FMOD::System* system;
	FMOD::ChannelGroup* handleGroupEffects = nullptr;
	std::unordered_map<std::string, FMOD::Sound*> sounds;
	std::vector<FMOD::Channel*> loopingSounds;
public:
	Sound2();
	virtual ~Sound2();

	static void init();
	static void update(float deltaTime);
	static void load(std::string fileName, bool stream = false);
	static void stopAll();

	static float getVolumeMaster();
	static void setVolumeMaster(float volume);
	static float getVolumeEffects();
	static void setVolumeEffects(float volume);

	static void play(const std::string& fileName, float volume = 1.0f, float pitch = 1.0f);

	static std::intptr_t playLooping(const std::string& fileName, float volume = 1.0f, float pitch = 1.0f);
	static bool changeLoopingVolume(std::intptr_t handle, float volume);
	static bool changeLoopingPitch(std::intptr_t handle, float pitch);
	static bool stopLooping(std::intptr_t handle);

	static void playSoundtrack(std::string fileNameCalm, std::string fileNameAggressive, float volume = 1.0f);
	static void stopSoundtrack();
	static void fadeSoundtrack(float aggressiveAmount);
	static void setVolumeSoundtrack(float volume);
	static float getVolumeSoundtrack();
	static void pauseSoundtrack(bool paused);
	static void stopAllLoops();

	//static void updateListener(Vector3 position, Vector3 lookAt, Vector3 up, Vector3 velocity);
};

#endif // !SOUND_H