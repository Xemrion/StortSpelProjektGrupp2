#pragma once
#include "GameObject.h"

enum class PowerUpType
{
	Time
};

class PowerUp : public GameObject
{
	PowerUpType type;
public:
	PowerUp();
	PowerUp(Vector3 position, PowerUpType type = PowerUpType::Time);
	~PowerUp();
	void init(PowerUpType type);
	void update(float time);
	PowerUpType getPowerUpType();
};
