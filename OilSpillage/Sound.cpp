#include "Sound.h"

Sound Sound::instance;

Sound::Sound()
{
	/*AUDIO_ENGINE_FLAGS eflags = AudioEngine_Default;
	#ifdef _DEBUG
	eflags = eflags | AudioEngine_Debug;
	#endif

	this->engine = AudioEngine(eflags);*/
}

Sound::~Sound()
{
}
