#include "Camera.h"
#include <DirectXMath.h>

Camera::Camera()
{
	this->nearDist = 1.f;
	this->farDist = 1000.f;
	this->fieldOfView = 90.f * (DirectX::XM_PI / 180);
	this->projection = DirectX::XMMatrixPerspectiveFovLH(this->fieldOfView, 16.f / 9.f, nearDist, farDist);
	this->nearHeight = float(tan(fieldOfView * 0.5f));
	this->nearWidth = nearHeight * (16.f / 9.f);
	this->farHeight = farDist * float(tan(fieldOfView * 0.5f));
	this->farWidth = farHeight * (16.f / 9.f);
}

Camera::Camera(float fovDegrees)
{
	this->nearDist = 1.f;
	this->farDist = 1000.f;
	this->fieldOfView = fovDegrees * (DirectX::XM_PI / 180);
	this->projection = DirectX::XMMatrixPerspectiveFovLH(this->fieldOfView, 16.f / 9.f, nearDist, farDist);
	this->nearHeight = tan(fieldOfView * 0.5f);
	this->nearWidth = nearHeight * (16.f / 9.f);
	this->farHeight = farDist * float(tan(fieldOfView * 0.5f));
	this->farWidth = farHeight * (16.f / 9.f);
}

Camera::Camera(float fovDegrees, float nearPlane, float farPlane)
{
	this->nearDist    = nearPlane;
	this->farDist     = farPlane;
	this->fieldOfView = fovDegrees * (DirectX::XM_PI / 180);
	this->projection  = DirectX::XMMatrixPerspectiveFovLH(this->fieldOfView, 16.f / 9.f, nearPlane, farPlane);
	this->nearHeight  = float(tan(fieldOfView * 0.5f));
	this->nearWidth   = nearHeight * (16.f / 9.f);
	this->farHeight   = farDist * float(tan(fieldOfView * 0.5f));
	this->farWidth    = farHeight * (16.f / 9.f);
}

Camera::~Camera()
{

}

void Camera::move(Vector3 dir)
{
	this->pos = this->pos + dir;
}

Vector3 Camera::getPos() const
{
	return this->pos;
}

void Camera::setPos(Vector3 newPos)
{
	this->pos = newPos;
}

Matrix Camera::getViewMatrix() const
{
	return XMMatrixLookAtLH(pos, pos + Vector3(0.0, -1.0, 0.0), Vector3(0.0, 0.0, 1.0));
}

Matrix Camera::getProjectionMatrix() const
{
	return projection;
}
