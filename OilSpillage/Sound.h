#ifndef SOUND_H
#define SOUND_H
#include <Audio.h>
#include <unordered_map>
#include "soloud.h"
#include "soloud_wav.h"
using namespace DirectX;

class Sound
{
private:
	struct SoundInstance
	{
		std::unique_ptr<SoundEffect> sound;
		std::unique_ptr<SoundEffectInstance> effectInstance;
		bool isLooping = false;
	};

	static std::unique_ptr<Sound> instance;

	float resetTimer;
	bool shouldReset;
	std::unique_ptr<AudioEngine> engine;
	std::unordered_map<std::wstring, SoundInstance> soundEffects;

	/*static std::unique_ptr<Sound> instance;*/

	SoLoud::Soloud soloud; // Engine core
	SoLoud::Wav soundTrackCalm; // One sample
	SoLoud::Wav soundTrackAggro; // One sample
	SoLoud::Wav soundTemp; // One sample
	SoLoud::Wav soundTemp2; // One sample

public:
	Sound();
	virtual ~Sound();

	static void Init();

	static void ShouldReset();
	static void Update(float deltaTime);
	static void Reset();

	static void loadSound(std::string fileName);
	static void PlaySoundEffect(std::string fileName);
	static void PlayLoopingSound(std::string fileName);
	static void StopLoopingSound(std::string fileName, bool immediate);
	static void changeVolume(std::wstring fileName, float volume);
	static void StopAllLoops(bool immediate);
	static void PauseSound(bool isPaused);
};

#endif // !SOUND_H
