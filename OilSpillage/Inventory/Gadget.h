#ifndef GADGET_H
#define GADGET_H

enum GadgetType { NITRO, EMP, GadgetNONE };

struct Gadget
{
	float lifeTime = 0.0f;
	GadgetType type = GadgetType::NITRO;
	float radius = 0.0f;
	float cooldown = 0.0f;
	float power = 0.0f; //speed = power
	float currentTime = 0.0f;
	float currentLifeTime = 3.0f;
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
		{3.0f,GadgetType::EMP,5.0f,30.0f,0.0f,0.0f,3.0f},
		{3.0f,GadgetType::NITRO,0.0f,30.0f,50.0f,0.0f,3.0f},
	};
	static Gadget getGadget(GadgetType type) {
		return gadgets[(int)type];
	};
};

#endif // !ITEMGADGET