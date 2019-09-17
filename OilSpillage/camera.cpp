#include "Camera.h"
#include <DirectXMath.h>

Camera::Camera()
{
	float nearPlane = 1.f;
	float farPlane = 1000.f;
	this->fieldOfView = 90.f * (DirectX::XM_PI / 180);
	this->projection = DirectX::XMMatrixPerspectiveFovLH(this->fieldOfView, 16.f / 9.f, nearPlane, farPlane);
}

Camera::Camera(float fovDegrees)
{
	float nearPlane = 1.f;
	float farPlane = 1000.f;
	this->fieldOfView = fovDegrees * (DirectX::XM_PI / 180);
	this->projection = DirectX::XMMatrixPerspectiveFovLH(this->fieldOfView, 16.f / 9.f, nearPlane, farPlane);
}

Camera::Camera(float fovDegrees, float nearPlane, float farPlane)
{
	this->fieldOfView = fovDegrees * (DirectX::XM_PI / 180);
	this->projection = DirectX::XMMatrixPerspectiveFovLH(this->fieldOfView, 16.f / 9.f, nearPlane, farPlane);
}

Camera::~Camera()
{

}

void Camera::move(Vector3 dir)
{
	this->pos = this->pos + dir;
}

Vector3 Camera::getPos()
{
	return this->pos;
}

void Camera::setPos(Vector3 newPos)
{
	this->pos = newPos;
}

Matrix Camera::getViewMatrix()
{
	return XMMatrixLookAtLH(pos, pos + Vector3(0.0, -1.0, 0.0), Vector3(0.0, 0.0, 1.0));
}

Matrix Camera::getProjectionMatrix()
{
	return projection;
}
