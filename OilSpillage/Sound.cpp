#include "Sound.h"

Sound Sound::instance;

Sound::Sound()
{
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
	delete this->engine;
}

void Sound::Update()
{
	if (!instance.engine)
		return;

	if (!instance.engine->Update() && instance.engine->IsCriticalError())
	{
		Sound::Reset();
	}
}

void Sound::Reset()
{
	if (!instance.engine)
		return;

	if (instance.engine->Reset())
	{
		// TODO: restart any looped sounds here
	}
}
