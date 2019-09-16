#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class Camera
{
private:
	Matrix view;
	Matrix projection;
	Vector3 pos = Vector3(0.0, 0.0, 0.0);
	Vector3 orientation = Vector3(0.0, 0.0, 0.0);
	float speed;
public:
	Camera();
	~Camera();
	void move(Vector3 dir);
	
	Vector3 getPos();
	void setPos(Vector3 pos);
	float getSpeed();
	void setSpeed(float speed);
	Matrix getViewMatrix();
};