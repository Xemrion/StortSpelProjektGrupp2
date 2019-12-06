#include "Sound.h"
#include <cassert>
#include <Windows.h>


std::unique_ptr<Sound> Sound::instance;

Sound::Sound()
{
	FMOD_RESULT result = FMOD::System_Create(&this->system); // Create the main system object.
	assert(result == FMOD_OK && "Could not create the sound system!");
	
	result = this->system->init(512, FMOD_INIT_NORMAL, nullptr); // Initialize FMOD.
	assert(result == FMOD_OK && "Could not init the sound system!");

	result = this->system->createChannelGroup("Effects", &this->handleGroupEffects);
	assert(result == FMOD_OK && "Could not init the effects sound group!");
}

Sound::~Sound()
{
	for (auto entry : this->sounds)
	{
		entry.second->release();
	}

	if (instance->soundtrack.filter)
	{
		instance->soundtrack.filter->release();
	}

	if (instance->soundtrack.handleGroup)
	{
		instance->soundtrack.handleGroup->release();
	}

	this->handleGroupEffects->release();
	this->system->release();
}

void Sound::init()
{
	Sound::instance = std::make_unique<Sound>();
}

void Sound::update(float deltaTime)
{
	instance->system->update();
}

void Sound::load(std::string fileName, bool stream)
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

	FMOD_RESULT result = instance->system->createSound((SOUND_PATH + fileName).c_str(), (stream ? FMOD_CREATESTREAM : FMOD_DEFAULT) | FMOD_LOOP_NORMAL, &exinfo, &sound);
	assert(result == FMOD_OK && "The sound could not be loaded!");

	if (result == FMOD_OK)
	{
		instance->sounds[fileName] = sound;
	}
}

void Sound::stopAll()
{
	FMOD::ChannelGroup* master = nullptr;
	instance->system->getMasterChannelGroup(&master);
	master->stop();

	instance->loopingSounds.clear();
	Sound::stopSoundtrack();
}

float Sound::getVolumeMaster()
{
	FMOD::ChannelGroup* master = nullptr;
	instance->system->getMasterChannelGroup(&master);

	float volume = 0.0f;
	master->getVolume(&volume);
	return volume;
}

void Sound::setVolumeMaster(float volume)
{
	FMOD::ChannelGroup* master = nullptr;
	instance->system->getMasterChannelGroup(&master);

	master->setVolume(volume);
}

float Sound::getVolumeEffects()
{
	float volume = 0.0f;
	instance->handleGroupEffects->getVolume(&volume);
	return volume;
}

void Sound::setVolumeEffects(float volume)
{
	instance->handleGroupEffects->setVolume(volume);
}

void Sound::play(const std::string& fileName, float volume, float pitch)
{
	if (instance->sounds.find(fileName) == instance->sounds.end())
	{
		Sound::load(fileName);
	}

	FMOD::Channel* channel = nullptr;
	FMOD_RESULT result = instance->system->playSound(instance->sounds[fileName], instance->handleGroupEffects, true, &channel);
	assert(result == FMOD_OK && "The sound could not be played!");

	if (result == FMOD_OK)
	{
		channel->setLoopCount(0);
		channel->setVolume(volume);
		channel->setPitch(pitch);
		channel->setPaused(false);
	}
}

std::intptr_t Sound::playLooping(const std::string& fileName, float volume, float pitch)
{
	if (instance->sounds.find(fileName) == instance->sounds.end())
	{
		Sound::load(fileName);
	}

	FMOD::Channel* channel = nullptr;
	FMOD_RESULT result = instance->system->playSound(instance->sounds[fileName], instance->handleGroupEffects, true, &channel);
	assert(result == FMOD_OK && "The sound could not be played!");
	
	if (result == FMOD_OK)
	{
		channel->setLoopCount(-1);
		channel->setVolume(volume);
		channel->setPitch(pitch);
		channel->setPaused(false);

		instance->loopingSounds.push_back(channel);
		return reinterpret_cast<intptr_t>(channel);
	}

	return -1;
}

bool Sound::changeLoopingVolume(std::intptr_t handle, float volume)
{
	FMOD::Channel* channel = reinterpret_cast<FMOD::Channel*>(handle);
	auto found = std::find(instance->loopingSounds.begin(), instance->loopingSounds.end(), channel);
	
	if (found != instance->loopingSounds.end())
	{
		channel->setVolume(max(volume, 0.0f));
		return true;
	}

	return false;
}

bool Sound::changeLoopingPitch(std::intptr_t handle, float pitch)
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

bool Sound::stopLooping(std::intptr_t handle)
{
	FMOD::Channel* channel = reinterpret_cast<FMOD::Channel*>(handle);
	auto found = std::find(instance->loopingSounds.begin(), instance->loopingSounds.end(), channel);

	if (found != instance->loopingSounds.end())
	{
		channel->stop();
		instance->loopingSounds.erase(found);
		return true;
	}

	return false;
}

void Sound::playSoundtrack(std::string fileNameCalm, std::string fileNameAggressive, float volume)
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
		Sound::load(fileNameCalm, true);
	if (instance->sounds.find(fileNameAggressive) == instance->sounds.end())
		Sound::load(fileNameAggressive, true);

	FMOD_RESULT result = instance->system->createChannelGroup("Soundtrack", &instance->soundtrack.handleGroup);
	assert(result == FMOD_OK && "Falied to create a ChannelGroup for the soundtrack!");

	result = instance->system->playSound(instance->sounds[fileNameCalm], instance->soundtrack.handleGroup, true, &instance->soundtrack.handleCalm);
	assert(result == FMOD_OK && "The sound could not be played!");
	result = instance->system->playSound(instance->sounds[fileNameAggressive], instance->soundtrack.handleGroup, true, &instance->soundtrack.handleAggressive);
	assert(result == FMOD_OK && "The sound could not be played!");

	instance->soundtrack.handleCalm->setVolume(1.0f);
	instance->soundtrack.handleAggressive->setVolume(1.0f);
	instance->soundtrack.handleGroup->setVolume(instance->soundtrack.volume);

	result = instance->system->createDSPByType(FMOD_DSP_TYPE_LOWPASS_SIMPLE, &instance->soundtrack.filter);
	assert(result == FMOD_OK && "The sound could not be played!");

	instance->soundtrack.filter->setParameterFloat(FMOD_DSP_LOWPASS_SIMPLE_CUTOFF, 10.0f);
	instance->soundtrack.handleAggressive->addDSP(0, instance->soundtrack.filter);

	instance->soundtrack.handleCalm->setPaused(false);
	instance->soundtrack.handleAggressive->setPaused(false);
}

void Sound::stopSoundtrack()
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
	if (instance->soundtrack.filter)
	{
		instance->soundtrack.filter->release();
		instance->soundtrack.filter = nullptr;
	}
	if (instance->soundtrack.handleGroup)
	{
		instance->soundtrack.handleGroup->stop();
		instance->soundtrack.handleGroup->release();
		instance->soundtrack.handleGroup = nullptr;
	}
}

void Sound::fadeSoundtrack(float aggressiveAmount)
{
	if (instance->soundtrack.filter)
		instance->soundtrack.filter->setParameterFloat(FMOD_DSP_LOWPASS_SIMPLE_CUTOFF, 10.0f + aggressiveAmount * (20000.f - 10.0f));
}

void Sound::setVolumeSoundtrack(float volume)
{
	instance->soundtrack.volume = volume;

	if (instance->soundtrack.handleGroup)
		instance->soundtrack.handleGroup->setVolume(volume);
}

float Sound::getVolumeSoundtrack()
{
	return instance->soundtrack.volume;
}

void Sound::pauseSoundtrack(bool paused)
{
	if (instance->soundtrack.handleGroup)
		instance->soundtrack.handleGroup->setPaused(paused);
}

void Sound::stopAllLoops()
{
	for (auto entry : instance->loopingSounds)
	{
		entry->stop();
	}

	instance->loopingSounds.clear();
}
