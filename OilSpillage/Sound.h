#ifndef SOUND_H
#define SOUND_H

#include <Audio.h>
#include <unordered_map>
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

public:
	Sound();
	virtual ~Sound();

	static void ShouldReset();
	static void Init();
	static void Update(float deltaTime);
	static void Reset();

	static void PlaySoundEffect(std::wstring fileName);
	static void PlayLoopingSound(std::wstring fileName);
	static void StopLoopingSound(std::wstring fileName, bool immediate);
	static void StopAllLoops(bool immediate);
	static void PauseSound(bool isPaused);
};

#endif // !SOUND_H
