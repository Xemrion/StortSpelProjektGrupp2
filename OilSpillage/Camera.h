#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class Camera
{
private:
	Matrix view;
	Vector3 pos = Vector3(0.0, 0.0, 0.0);
public:
	Camera();
	~Camera();
	void move(Vector3 dir);
	
	Vector3 getPos();
	void setPos(Vector3 pos);
	Matrix getViewMatrix();
};