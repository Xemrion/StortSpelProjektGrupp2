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
public:
	Vehicle();
	virtual ~Vehicle();

	void init(Graphics& graphics);
	void update(float deltaTime, std::unique_ptr<DirectX::Keyboard> &kb);
};