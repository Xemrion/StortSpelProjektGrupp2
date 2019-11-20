#include "Sound.h"
#include "Game.h"

std::unique_ptr<Sound> Sound::instance;

Sound::Sound()
{
}

Sound::~Sound()
{
}

void Sound::init()
{
	instance = std::make_unique<Sound>();
	instance->soloud.init(SoLoud::Soloud::FLAGS::LEFT_HANDED_3D);
	instance->soloud.setMaxActiveVoiceCount(64);
}

void Sound::deinit()
{
	instance->soloud.deinit();
}

void Sound::update(float deltaTime)
{
	instance->soloud.update3dAudio();
}

void Sound::stopAll()
{
	instance->soloud.stopAll();
	instance->loopingSounds.clear();
}

void Sound::load(const std::string& fileName)
{
	if (instance->sounds.find(fileName) == instance->sounds.end())
	{
		instance->sounds[fileName] = std::make_unique<SoLoud::Wav>();
		instance->sounds[fileName]->load(fileName.c_str());
	}
}

void Sound::sayText(const std::string& text)
{
	instance->speech.setText(text.c_str());
	instance->soloud.play(instance->speech);
}

void Sound::play(const std::string& fileName, float volume, float pitch)
{
	if (instance->sounds.find(fileName) == instance->sounds.end())
	{
		instance->sounds[fileName] = std::make_unique<SoLoud::Wav>();
		instance->sounds[fileName]->load(fileName.c_str());
	}

	int handle = instance->soloud.playClocked(Game::getDeltaTime(), *instance->sounds[fileName].get(), max(volume, 0.0f));

	if (pitch != 1.0f)
	{
		instance->soloud.setRelativePlaySpeed(handle, pitch);
	}
}

void Sound::play3d(const std::string& fileName, Vector3 position, Vector3 velocity, float volume, float pitch)
{
	if (instance->sounds.find(fileName) == instance->sounds.end())
	{
		instance->sounds[fileName] = std::make_unique<SoLoud::Wav>();
		instance->sounds[fileName]->load(fileName.c_str());
	}

	int handle = instance->soloud.play3dClocked(Game::getDeltaTime(), *instance->sounds[fileName].get(), position.x, position.y, position.z, velocity.x, velocity.y, velocity.z, max(volume, 0.0f));
	instance->soloud.set3dSourceMinMaxDistance(handle, 20.0f, 40.0f);
	instance->soloud.set3dSourceAttenuation(handle, SoLoud::AudioSource::LINEAR_DISTANCE, 1.0f);

	if (pitch != 1.0f)
	{
		instance->soloud.setRelativePlaySpeed(handle, pitch);
	}
}

int Sound::playLooping(const std::string& fileName, float volume, float pitch)
{
	if (instance->sounds.find(fileName) == instance->sounds.end())
	{
		instance->sounds[fileName] = std::make_unique<SoLoud::Wav>();
		instance->sounds[fileName]->load(fileName.c_str());
	}

	int handle = instance->soloud.playClocked(Game::getDeltaTime(), *instance->sounds[fileName].get(), max(volume, 0.0f));
	instance->soloud.setLooping(handle, true);

	if (pitch != 1.0f)
	{
		instance->soloud.setRelativePlaySpeed(handle, pitch);
	}

	instance->loopingSounds.push_back(handle);
	return handle;
}

int Sound::play3dLooping(const std::string& fileName, Vector3 position, Vector3 velocity, float volume, float pitch)
{
	if (instance->sounds.find(fileName) == instance->sounds.end())
	{
		instance->sounds[fileName] = std::make_unique<SoLoud::Wav>();
		instance->sounds[fileName]->load(fileName.c_str());
	}

	int handle = instance->soloud.play3dClocked(Game::getDeltaTime(), *instance->sounds[fileName].get(), position.x, position.y, position.z, velocity.x, velocity.y, velocity.z, max(volume, 0.0f));
	instance->soloud.set3dSourceMinMaxDistance(handle, 20.0f, 40.0f);
	instance->soloud.set3dSourceAttenuation(handle, SoLoud::AudioSource::LINEAR_DISTANCE, 1.0f);
	instance->soloud.setLooping(handle, true);

	if (pitch != 1.0f)
	{
		instance->soloud.setRelativePlaySpeed(handle, pitch);
	}

	instance->loopingSounds.push_back(handle);
	return handle;
}

void Sound::update3dLooping(int handle, Vector3 position, Vector3 velocity)
{
	for (auto loop = instance->loopingSounds.begin(); loop != instance->loopingSounds.end(); loop++)
	{
		if ((*loop) == handle)
		{
			instance->soloud.set3dSourceParameters(handle, position.x, position.y, position.z, velocity.x, velocity.y, velocity.z);
			return;
		}
	}
}

bool Sound::changeLoopingVolume(int handle, float volume)
{
	for (auto loop = instance->loopingSounds.begin(); loop != instance->loopingSounds.end(); loop++)
	{
		if ((*loop) == handle)
		{
			instance->soloud.setVolume(handle, max(volume, 0.0f));
			return true;
		}
	}

	return false;
}

bool Sound::changeLoopingPitch(int handle, float pitch)
{
	for (auto loop = instance->loopingSounds.begin(); loop != instance->loopingSounds.end(); loop++)
	{
		if ((*loop) == handle)
		{
			instance->soloud.setRelativePlaySpeed(handle, pitch);
			return true;
		}
	}

	return false;
}

bool Sound::stopLooping(int handle, float fadeOutTime)
{
	for (auto loop = instance->loopingSounds.begin(); loop != instance->loopingSounds.end(); loop++)
	{
		if ((*loop) == handle)
		{
			instance->soloud.fadeVolume(handle, 0, fadeOutTime);
			instance->soloud.scheduleStop(handle, fadeOutTime);
			instance->loopingSounds.erase(loop);
			return true;
		}
	}

	return false;
}

void Sound::playSoundtrack(std::string fileNameCalm, std::string fileNameAggressive, float volume)
{
	if (instance->sounds.find(fileNameCalm) == instance->sounds.end())
	{
		instance->sounds[fileNameCalm] = std::make_unique<SoLoud::Wav>();
		instance->sounds[fileNameCalm]->load(fileNameCalm.c_str());
	}

	if (instance->sounds.find(fileNameAggressive) == instance->sounds.end())
	{
		instance->sounds[fileNameAggressive] = std::make_unique<SoLoud::Wav>();
		instance->sounds[fileNameAggressive]->load(fileNameAggressive.c_str());
	}

	Sound::stopSoundtrack();
	instance->soloud.destroyVoiceGroup(instance->soundtrack.handleGroup);

	instance->soundtrack.volume = volume;
	instance->soundtrack.handleCalm = instance->soloud.playBackground(*instance->sounds[fileNameCalm].get(), max(volume, 0.0f), true);
	instance->soundtrack.handleAggressive = instance->soloud.playBackground(*instance->sounds[fileNameAggressive].get(), 0.0f, true);

	instance->soundtrack.handleGroup = instance->soloud.createVoiceGroup();
	instance->soloud.addVoiceToGroup(instance->soundtrack.handleGroup, instance->soundtrack.handleCalm);
	instance->soloud.addVoiceToGroup(instance->soundtrack.handleGroup, instance->soundtrack.handleAggressive);

	instance->soloud.setProtectVoice(instance->soundtrack.handleCalm, true);
	instance->soloud.setProtectVoice(instance->soundtrack.handleAggressive, true);
	instance->soloud.setProtectVoice(instance->soundtrack.handleGroup, true);

	instance->soloud.setLooping(instance->soundtrack.handleGroup, true);
	instance->soloud.setPause(instance->soundtrack.handleGroup, false);
}

void Sound::fadeSoundtrack(bool toAgressive, float fadeTime)
{
	float aggressiveFactor = instance->soloud.getVolume(instance->soundtrack.handleAggressive) / instance->soundtrack.volume;

	if (toAgressive)
	{
		aggressiveFactor = 1 - aggressiveFactor;
	}

	instance->soloud.fadeVolume(instance->soundtrack.handleAggressive, toAgressive ? instance->soundtrack.volume : 0.0f, fadeTime * aggressiveFactor);
}

void Sound::stopSoundtrack(float fadeOutTime)
{
	instance->soloud.fadeVolume(instance->soundtrack.handleGroup, 0, fadeOutTime);
	instance->soloud.scheduleStop(instance->soundtrack.handleGroup, fadeOutTime);
}

void Sound::updateListener(Vector3 position, Vector3 lookAt, Vector3 up, Vector3 velocity)
{
	instance->soloud.set3dListenerParameters(position.x, position.y, position.z, lookAt.x, lookAt.y, lookAt.z, up.x, up.y, up.z, velocity.x, velocity.y, velocity.z);
}
