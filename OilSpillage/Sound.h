#ifndef SOUND_H
#define SOUND_H

#include <Audio.h>
using namespace DirectX;

class Sound
{
private:
	static Sound instance;
	//AudioEngine engine;

	Sound();

public:
	virtual ~Sound();
};

#endif // !SOUND_H
