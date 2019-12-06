#include "Sound.h"
#include <cassert>
#include <Windows.h>

/*
void Sound::fadeSoundtrack(bool toAgressive, float fadeTime)
{
	//Get the aggressive factor (the time of the fade that is left) so this function can be called every frame if needed.
	float aggressiveFactor = (instance->soloud.getFilterParameter(instance->soundtrack.handleAggressive, 0, SoLoud::BiquadResonantFilter::FREQUENCY) - 10.0f) / 10000.0f;
	if (toAgressive) aggressiveFactor = 1 - aggressiveFactor;

	instance->soloud.fadeFilterParameter(instance->soundtrack.handleAggressive, 0, SoLoud::BiquadResonantFilter::FREQUENCY, toAgressive ? 10000.0f : 10.0f, fadeTime * aggressiveFactor);
}
*/

std::unique_ptr<Sound2> Sound2::instance;

Sound2::Sound2()
{
	FMOD_RESULT result = FMOD::System_Create(&this->system); // Create the main system object.
	assert(result == FMOD_OK && "Could not create the sound system!");
	
	result = this->system->init(512, FMOD_INIT_NORMAL, nullptr); // Initialize FMOD.
	assert(result == FMOD_OK && "Could not init the sound system!");
}

Sound2::~Sound2()
{
	for (auto entry : this->sounds)
	{
		entry.second->release();
	}

	this->system->release();
}

void Sound2::init()
{
	Sound2::instance = std::make_unique<Sound2>();
}

void Sound2::update(float deltaTime)
{
	instance->system->update();
}

void Sound2::load(std::string fileName, bool stream)
{
	//Remove potential copies
	if ((fileName[0] == '.' && (fileName[1] == '/' || fileName[1] == '\\')) || fileName[0] == '/' || fileName[0] == '\\' || fileName.find('\\') != std::string::npos)
	{
		assert(false && "Don't use './' or '.\' when specifying the sound path! Don't use '\' at all either!");
	}

	//If that sound is already loaded then don't do anything
	if (instance->sounds.find(fileName) != instance->sounds.end()) return;

	FMOD_CREATESOUNDEXINFO exinfo = {};
	exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	FMOD::Sound* sound = nullptr;

	FMOD_RESULT result = instance->system->createSound((SOUND_PATH + fileName).c_str(), stream ? FMOD_CREATESTREAM : FMOD_DEFAULT, &exinfo, &sound);
	assert(result == FMOD_OK && "The sound could not be loaded!");

	if (result == FMOD_OK)
	{
		instance->sounds[fileName] = sound;
	}
}

void Sound2::stopAll()
{
	FMOD::ChannelGroup* master = nullptr;
	instance->system->getMasterChannelGroup(&master);
	master->stop();

	instance->loopingSounds.clear();
}

float Sound2::getMasterVolume()
{
	FMOD::ChannelGroup* master = nullptr;
	instance->system->getMasterChannelGroup(&master);

	float volume = 0.0f;
	master->getVolume(&volume);
	return volume;
}

void Sound2::setMasterVolume(float volume)
{
	FMOD::ChannelGroup* master = nullptr;
	instance->system->getMasterChannelGroup(&master);

	master->setVolume(volume);
}

void Sound2::play(const std::string& fileName, float volume, float pitch)
{
	if (instance->sounds.find(fileName) == instance->sounds.end())
	{
		Sound2::load(fileName);
	}

	FMOD::Channel* channel = nullptr;
	FMOD_RESULT result = instance->system->playSound(instance->sounds[fileName], nullptr, false, &channel);
	assert(result == FMOD_OK && "The sound could not be played!");

	if (result == FMOD_OK)
	{
		OutputDebugString(("Played '" + fileName + "'\n").c_str());
		channel->setLoopCount(0);
		channel->setVolume(volume);
		channel->setPitch(pitch);
	}
}

std::intptr_t Sound2::playLooping(const std::string& fileName, float volume, float pitch)
{
	if (instance->sounds.find(fileName) == instance->sounds.end())
	{
		Sound2::load(fileName);
	}

	FMOD::Channel* channel = nullptr;
	FMOD_RESULT result = instance->system->playSound(instance->sounds[fileName], nullptr, false, &channel);
	assert(result == FMOD_OK && "The sound could not be played!");
	
	if (result == FMOD_OK)
	{
		OutputDebugString(("Played loop '" + fileName + "'\n").c_str());
		channel->setLoopCount(-1);
		channel->setVolume(volume);
		channel->setPitch(pitch);

		instance->loopingSounds.push_back(channel);
		return reinterpret_cast<intptr_t>(channel);
	}

	return -1;
}

bool Sound2::changeLoopingVolume(std::intptr_t handle, float volume)
{
	FMOD::Channel* channel = reinterpret_cast<FMOD::Channel*>(handle);
	auto found = std::find(instance->loopingSounds.begin(), instance->loopingSounds.end(), channel);
	
	if (found != instance->loopingSounds.end())
	{
		channel->setVolume(volume);
		return true;
	}

	return false;
}

bool Sound2::changeLoopingPitch(std::intptr_t handle, float pitch)
{
	FMOD::Channel* channel = reinterpret_cast<FMOD::Channel*>(handle);
	auto found = std::find(instance->loopingSounds.begin(), instance->loopingSounds.end(), channel);

	if (found != instance->loopingSounds.end())
	{
		channel->setPitch(pitch);
		return true;
	}

	return false;
}

bool Sound2::stopLooping(std::intptr_t handle)
{
	FMOD::Channel* channel = reinterpret_cast<FMOD::Channel*>(handle);
	auto found = std::find(instance->loopingSounds.begin(), instance->loopingSounds.end(), channel);

	if (found != instance->loopingSounds.end())
	{
		instance->loopingSounds.erase(found);
		return true;
	}

	return false;
}

void Sound2::playSoundtrack(std::string fileNameCalm, std::string fileNameAggressive, float volume)
{
	if (instance->soundtrack.handleCalm)
		instance->soundtrack.handleCalm->stop();
	if (instance->soundtrack.handleAggressive)
		instance->soundtrack.handleAggressive->stop();
	if (instance->soundtrack.handleGroup)
	{
		instance->soundtrack.handleGroup->stop();
		instance->soundtrack.handleGroup->release();
	}

	if (instance->sounds.find(fileNameCalm) == instance->sounds.end())
		Sound2::load(fileNameCalm, true);
	if (instance->sounds.find(fileNameAggressive) == instance->sounds.end())
		Sound2::load(fileNameAggressive, true);

	FMOD_RESULT result = instance->system->createChannelGroup("Soundtrack", &instance->soundtrack.handleGroup);
	assert(result == FMOD_OK && "Falied to create a ChannelGroup for the soundtrack!");

	result = instance->system->playSound(instance->sounds[fileNameCalm], instance->soundtrack.handleGroup, true, &instance->soundtrack.handleCalm);
	assert(result == FMOD_OK && "The sound could not be played!");
	result = instance->system->playSound(instance->sounds[fileNameAggressive], instance->soundtrack.handleGroup, true, &instance->soundtrack.handleAggressive);
	assert(result == FMOD_OK && "The sound could not be played!");

	result = instance->soundtrack.handleGroup->setVolume(volume);
	assert(result == FMOD_OK && "Falied to create a ChannelGroup for the soundtrack!");
	result = instance->soundtrack.handleGroup->setPaused(false);
	assert(result == FMOD_OK && "Falied to create a ChannelGroup for the soundtrack!");
}

void Sound2::stopSoundtrack()
{
	if (instance->soundtrack.handleCalm)
	{
		instance->soundtrack.handleCalm->stop();
		instance->soundtrack.handleCalm = nullptr;
	}
	if (instance->soundtrack.handleAggressive)
	{
		instance->soundtrack.handleAggressive->stop();
		instance->soundtrack.handleAggressive = nullptr;
	}
	if (instance->soundtrack.handleGroup)
	{
		instance->soundtrack.handleGroup->stop();
		instance->soundtrack.handleGroup->release();
		instance->soundtrack.handleGroup = nullptr;
	}
}

void Sound2::changeSoundtrackVolume(float volume)
{
	if (instance->soundtrack.handleGroup)
		instance->soundtrack.handleGroup->setVolume(volume);
}

void Sound2::stopAllLoops()
{
	for (auto entry : instance->loopingSounds)
	{
		entry->stop();
	}

	instance->loopingSounds.clear();
}
