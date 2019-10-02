#pragma once
#include "window.h"
#include "graphics.h"
#include"Keyboard.h"
#include"Mouse.h"
class Vehicle
{
private:
	GameObject* vehicle;
	GameObject* bodyRotation;
	GameObject* bodyRotationPoint;

	static const int bulletCount = 16;
	float leftoverTime;
	float bulletLifetime;
	float bulletSpeed;
	float fireSpeed;
	struct Bullet
	{
		Vector3 dir;
		float speed = 0.0f;
		float timeLeft = 0.0f;
		GameObject* obj = nullptr;
	} bullets[bulletCount];

	Vector3 bodyPivot;
	DirectX::XMFLOAT2 velocity;
	float strength;
	float add;
	
	DirectX::XMFLOAT3 accelerator;
	float acceleratorTempX;
	float acceleratorTempZ;
	
	float targetRotation;
	int drivingMode;
	float topSpeed;
	float counter = 0.0f;
	float rotateAcceleration = 0.0f;
	float rotationSmoother = 1.0f;
	float tempTargetRotation;
	bool rotationDirection;
public:
	Vehicle();
	virtual ~Vehicle();

	void init();
	void update(float deltaTime, Vector2 collisionDir);
	GameObject* getVehicle() { return this->vehicle; }
	float getAcceleratorX();

	void setDrivingMode(int i);
	bool getDrivingMode();
	Vector3 getVelocity();
	//void onCollision(Vector2 direction);

};