#pragma once
#include "GameObject.h"

enum class PowerUpType
{
	Time,
	Health,
	Speed,
	Star,
	Length // only for creating arrays
};

class PowerUp : public GameObject
{
	PowerUpType type;
	static float powerUpDurations[(int)PowerUpType::Length];
	float respawnTime;
	float currentRespawnTimer;
	float time;
	int soundHandle;
	void loadModel();
	void clone(const PowerUp& p);
	Physics* physics;
	float acceleration;
	void initRigidBody();
public:
	PowerUp();
	PowerUp(Vector3 position, Physics* physics, PowerUpType type = PowerUpType::Time, float respawnTime = 10000000.0);
	PowerUp(const PowerUp& p);
	PowerUp(PowerUp&& p) noexcept;
	~PowerUp();
	PowerUp& operator=(const PowerUp& p);
	PowerUp& operator=(PowerUp&& p) noexcept;
	void update(float time, Vector3 playerPos);
	void activate();
	void deactivate();
	bool isActive() const;
	float getCurrentRespawnTimer() const;
	PowerUpType getPowerUpType() const;
};
