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
		this->engine = new AudioEngine(eflags);
	}
	catch (const std::exception&)
	{
		this->engine = nullptr;
	}
}

Sound::~Sound()
{
	this->engine->Suspend();
	delete this->engine;

	for (auto i = this->soundEffects.begin(); i != this->soundEffects.end(); i++)
	{
		delete i->second;
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
				// TODO: restart any looped sounds here
			}
		}
	}
	else if (!instance->engine->Update() && instance->engine->IsCriticalError())
	{
		instance->shouldReset = true;
	}
}

void Sound::PlaySoundEffect(std::wstring fileName)
{
	if (instance->soundEffects.find(fileName) == instance->soundEffects.end())
	{
		try
		{
			SoundEffect* effect = new SoundEffect(instance->engine, fileName.c_str());
			instance->soundEffects[fileName] = effect;
		}
		catch (const std::exception&)
		{
			return;
		}
	}

	instance->soundEffects[fileName]->Play();
}
