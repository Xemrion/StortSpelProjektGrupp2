#include "Sound.h"

std::unique_ptr<Sound> Sound::instance;

Sound::Sound()
{
	this->resetTimer = 0.0f;
	this->shouldReset = false;

	AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
	#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
	#endif

	try
	{
		this->engine = std::make_unique<AudioEngine>(eflags);
	}
	catch (const std::exception&) {}
}

Sound::~Sound()
{
	this->engine->Suspend();

	for (auto i = this->soundEffects.begin(); i != this->soundEffects.end(); i++)
	{
		i->second.effectInstance.reset();
	}
}

void Sound::ShouldReset()
{
	instance->shouldReset = true;
}

void Sound::Init()
{
	instance = std::make_unique<Sound>();
}

void Sound::Update(float deltaTime)
{
	if (!instance->engine)
		return;

	if (instance->shouldReset)
	{
		instance->resetTimer += deltaTime;

		if (instance->resetTimer > 3.0f) //3 second wait time until reset.
		{
			instance->resetTimer = 0;
			instance->shouldReset = false;

			if (instance->engine->Reset())
			{
				for (auto i = instance->soundEffects.begin(); i != instance->soundEffects.end(); i++)
				{
					if (i->second.isLooping)
					{
						i->second.effectInstance->Play(true);
					}
				}
			}
		}
	}
	else if (!instance->engine->Update() && instance->engine->IsCriticalError())
	{
		instance->shouldReset = true;
	}
}

void Sound::Reset()
{
	instance->engine->Reset();
}

void Sound::PlaySoundEffect(std::wstring fileName)
{
	if (instance->soundEffects.find(fileName) == instance->soundEffects.end())
	{
		try
		{
			instance->soundEffects[fileName].sound = std::make_unique<SoundEffect>(instance->engine.get(), fileName.c_str());
			instance->soundEffects[fileName].effectInstance = instance->soundEffects[fileName].sound->CreateInstance();
		}
		catch (const std::exception&)
		{
			return;
		}
	}

	instance->soundEffects[fileName].sound->Play();
}

void Sound::PlayLoopingSound(std::wstring fileName)
{
	if (instance->soundEffects.find(fileName) == instance->soundEffects.end())
	{
		try
		{
			instance->soundEffects[fileName].sound = std::make_unique<SoundEffect>(instance->engine.get(), fileName.c_str());
			instance->soundEffects[fileName].effectInstance = instance->soundEffects[fileName].sound->CreateInstance();
		}
		catch (const std::exception&)
		{
			return;
		}
	}

	instance->soundEffects[fileName].effectInstance->Stop(true);
	instance->soundEffects[fileName].effectInstance->Play(true);
	instance->soundEffects[fileName].isLooping = true;
}

void Sound::StopLoopingSound(std::wstring fileName, bool immediate)
{
	if (instance->soundEffects.find(fileName) != instance->soundEffects.end())
	{
		instance->soundEffects[fileName].effectInstance->Stop(immediate);
		instance->soundEffects[fileName].isLooping = false;
	}
}

void Sound::changeVolume(std::wstring fileName, float volume)
{
	if (instance->soundEffects.find(fileName) != instance->soundEffects.end())
	{
		instance->soundEffects[fileName].effectInstance->SetVolume(volume);
	}
}

void Sound::StopAllLoops(bool immediate)
{
	for (auto i = instance->soundEffects.begin(); i != instance->soundEffects.end(); i++)
	{
		i->second.effectInstance->Stop(immediate);
	}
}

void Sound::PauseSound(bool isPaused)
{
	isPaused ? instance->engine->Suspend() : instance->engine->Resume();
}
