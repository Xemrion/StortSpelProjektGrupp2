#ifndef SOUND_H
#define SOUND_H

#include <memory>
#include <unordered_map>
#include <vector>
#include <d3d11.h>
#include <SimpleMath.h>
#include "soloud.h"
#include "soloud_wav.h"
#include "soloud_wavstream.h"
#include "soloud_speech.h"
#include "soloud_biquadresonantfilter.h"

using namespace DirectX::SimpleMath;

class Sound
{
private:
	struct Soundtrack
	{
		std::unique_ptr<SoLoud::WavStream> soundCalm;
		std::unique_ptr<SoLoud::WavStream> soundAggressive;
		std::unique_ptr<SoLoud::BiquadResonantFilter> filter;
		float volume = 1.0f;
		int handleGroup = 0;
		int handleCalm = 0;
		int handleAggressive = 0;
	};

	static std::unique_ptr<Sound> instance;

	SoLoud::Soloud soloud;
	std::unordered_map<std::string, std::unique_ptr<SoLoud::Wav>> sounds;
	std::vector<int> loopingSounds;
	SoLoud::Speech speech;
	Soundtrack soundtrack;
public:
	Sound();
	virtual ~Sound();

	static void init();
	static void deinit();
	static void update(float deltaTime);
	static void stopAll();
	static void load(const std::string& fileName);
	static float getMasterVolume();
	static void setMasterVolume(float volume);

	static void sayText(const std::string& text);

	static void play(const std::string& fileName, float volume = 1.0f, float pitch = 1.0f);
	static void play3d(const std::string& fileName, Vector3 position = Vector3::Zero, Vector3 velocity = Vector3::Zero, float volume = 1.0f, float pitch = 1.0f);

	static int playLooping(const std::string& fileName, float volume = 1.0f, float pitch = 1.0f);
	static int play3dLooping(const std::string& fileName, Vector3 position = Vector3::Zero, Vector3 velocity = Vector3::Zero, float volume = 1.0f, float pitch = 1.0f);
	static void update3dLooping(int handle, Vector3 position, Vector3 velocity);
	static bool changeLoopingVolume(int handle, float volume);
	static bool changeLoopingPitch(int handle, float pitch);
	static bool stopLooping(int handle, float fadeOutTime = 0.0f);

	static void playSoundtrack(std::string fileNameCalm, std::string fileNameAggressive, float volume = 1.0f);
	static void fadeSoundtrack(bool toAgressive, float fadeTime);
	static void stopSoundtrack(float fadeOutTime = 0.0f);
	static void changeSoundtrackVolume(float volume);

	static void stopAllSoundsExceptSoundtrack();

	static void updateListener(Vector3 position, Vector3 lookAt, Vector3 up, Vector3 velocity);
};

#endif // !SOUND_H