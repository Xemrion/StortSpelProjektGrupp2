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
	instance->soloud.deinit(); // Clean up!
}

void Sound::ShouldReset()
{
	instance->shouldReset = true;
}

void Sound::Init()
{
	instance = std::make_unique<Sound>();
	instance->soloud.init();
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

void Sound::loadSound(std::string fileName)
{
	/*if (instance->soundEffects.find(fileName) == instance->soundEffects.end())
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
	}*/
	instance->soundTemp.load(fileName.c_str()); // Load a f i l e
}

void Sound::PlaySoundEffect(std::string fileName)
{
	/*if (instance->soundEffects.find(fileName) == instance->soundEffects.end())
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

	instance->soundEffects[fileName].sound->Play();*/
	instance->soundTemp.setLooping(false);
	instance->soundTemp.load(fileName.c_str()); // Load a f i l e
	instance->soloud.play(instance->soundTemp); // Pl ay i t
}

void Sound::PlayLoopingSound(std::string fileName)
{
	/*if (instance->soundEffects.find(fileName) == instance->soundEffects.end())
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
	instance->soundEffects[fileName].isLooping = true;*/
	instance->soundTemp2.load(fileName.c_str()); // Load a f i l e
	instance->soundTemp2.setLooping(true);
	instance->soloud.play(instance->soundTemp2); // Pl ay i t
	
}

void Sound::StopLoopingSound(std::string fileName, bool immediate)
{
	/*if (instance->soundEffects.find(fileName) != instance->soundEffects.end())
	{
		instance->soundEffects[fileName].effectInstance->Stop(immediate);
		instance->soundEffects[fileName].isLooping = false;
	}*/
	
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
