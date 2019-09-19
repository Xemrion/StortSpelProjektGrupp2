#ifndef SOUND_H
#define SOUND_H

#include <Audio.h>
#include <unordered_map>
using namespace DirectX;

class Sound
{
private:
	static std::unique_ptr<Sound> instance;

	float resetTimer;
	bool shouldReset;
	AudioEngine* engine;
	std::unordered_map<std::wstring, SoundEffect*> soundEffects;

public:
	Sound();
	virtual ~Sound();

	static void ShouldReset();
	static void Init();
	static void Update(float deltaTime);
	static void PlaySoundEffect(std::wstring fileName);
};

#endif // !SOUND_H
