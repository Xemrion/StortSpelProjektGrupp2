#ifndef GADGET_H
#define GADGET_H

enum GadgetType { GadgetNONE, NITRO, EMP };

struct Gadget
{
	float lifeTime = 5.0f;
	GadgetType type = GadgetType::GadgetNONE;
	float radius = 0.0f;
	float cooldown = 30.0f;
	float power = 75.0f; //speed = power
	float currentTime = 0.0f;
	float currentLifeTime = 5.0f;
	bool enable = false;
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
		{10.0f,GadgetType::NITRO,0.0f,6.0f,75.0f,0.0f,3.0f,false},
		{5.0f,GadgetType::EMP,15.0f,6.0f,0.0f,0.0f,3.0f,false},
	};
	static Gadget getGadget(GadgetType type) {
		return gadgets[(int)type];
	};
};

#endif // !ITEMGADGET