#pragma once
#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

struct Frustum {
	Vector4 nearPlane;
	Vector4 farPlane;
	Vector4 leftPlane;
	Vector4 rightPlane;
	Vector4 topPlane;
	Vector4 bottomPlane;
};

class Camera
{
private:
	Matrix view;
	Matrix projection;
	float fieldOfView;
	float nearDist;
	float farDist;
	float nearHeight;
	float nearWidth;
	float farHeight;
	float farWidth;
	Vector3 pos = Vector3(0.0, 0.0, 0.0);
public:
	Camera();
	Camera(float fovDegrees);
	Camera(float fovDegrees, float nearPlane, float farPlane);
	~Camera();
	void move(Vector3 dir);
	
	Vector3 getPos() const;
	void setPos(Vector3 pos);
	Matrix getViewMatrix() const;
	Matrix getProjectionMatrix() const;
	Frustum getFrustum() const;
};