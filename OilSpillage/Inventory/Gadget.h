#pragma once
#include <cstdlib>
enum GadgetType { NITRO, EMP, NONE };

	struct Gadget
	{
		float lifeTime = 0.0f;
		GadgetType type = GadgetType::NITRO;
		float radius = 0.0f;
		float cooldown = 0.0f;
		float power = 0.0f; //speed = power

	};
	static float random(int max, int min)
	{
		float value = rand() % max + min;
		return value;
	}

	class GadgetHandler
	{
	public:
		static constexpr Gadget gadgets[] =
		{
			Gadget(),
			{0.0f,GadgetType::EMP,0.0f,0.0f,0.0f}
		};
	};