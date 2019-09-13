#pragma once
#include "window.h"
#include "graphics.h"
#include"Keyboard.h"
#include"Mouse.h"
class Vehicle
{
private:
	GameObject* vehicle;
	DirectX::XMFLOAT2 velocity;
	
	float targetRotation;
	int drivingMode;
	float topSpeed;
	float counter = 0.0f;
	float rotateAcceleration = 0.0f;
	float rotationSmoother = 1.0f;
	float tempTargetRotation;
public:
	Vehicle();
	virtual ~Vehicle();

	void init();
	void update(float deltaTime, std::unique_ptr<DirectX::Keyboard> &kb);
};