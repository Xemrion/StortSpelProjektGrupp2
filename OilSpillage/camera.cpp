#include "Camera.h"
#include <DirectXMath.h>

Camera::Camera()
{
	this->nearDist = 1.f;
	this->farDist = 1000.f;
	this->fieldOfView = 90.f * (DirectX::XM_PI / 180);
	this->projection = DirectX::XMMatrixPerspectiveFovLH(this->fieldOfView, 16.f / 9.f, nearDist, farDist);
	this->nearHeight = tan(fieldOfView * 0.5f);
	this->nearWidth = nearHeight * (16.f / 9.f);
	this->farHeight = farDist * tan(fieldOfView * 0.5f);
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
	this->farHeight = farDist * tan(fieldOfView * 0.5f);
	this->farWidth = farHeight * (16.f / 9.f);
}

Camera::Camera(float fovDegrees, float nearPlane, float farPlane)
{
	this->nearDist = nearPlane;
	this->farDist = farPlane;
	this->fieldOfView = fovDegrees * (DirectX::XM_PI / 180);
	this->projection = DirectX::XMMatrixPerspectiveFovLH(this->fieldOfView, 16.f / 9.f, nearPlane, farPlane);
	this->nearHeight = tan(fieldOfView * 0.5f);
	this->nearWidth = nearHeight * (16.f / 9.f);
	this->farHeight = farDist * tan(fieldOfView * 0.5f);
	this->farWidth = farHeight * (16.f / 9.f);
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

Frustum Camera::getFrustum() const
{
	Frustum frustum;
	Matrix invView = this->getViewMatrix().Invert();
	Vector4 viewDir = DirectX::XMVector4Transform(Vector4(0.0, 0.0, 1.0, 0.0), invView);
	Vector4 viewUp = DirectX::XMVector4Transform(Vector4(0.0, 1.0, 0.0, 0.0), invView);
	Vector4 viewRight = DirectX::XMVector4Transform(Vector4(1.0, 0.0, 0.0, 0.0), invView);
	Vector4 cameraPlane = DirectX::XMPlaneFromPointNormal(pos, viewDir);
	frustum.nearPlane = DirectX::XMPlaneFromPointNormal(pos + viewDir * this->nearDist, -viewDir);
	frustum.farPlane = DirectX::XMPlaneFromPointNormal(pos + viewDir * this->farDist, viewDir);

	Vector3 nearCenter = pos + viewDir * nearDist;
	Vector3 farCenter = pos + viewDir * farDist;

	Vector3 nearTopLeft = nearCenter + Vector3(viewUp) * nearHeight - Vector3(viewRight) * nearWidth;
	Vector3 nearTopRight = nearCenter + Vector3(viewUp) * nearHeight + Vector3(viewRight) * nearWidth;
	Vector3 nearBottomLeft = nearCenter - Vector3(viewUp) * nearHeight - Vector3(viewRight) * nearWidth;
	Vector3 nearBottomRight = nearCenter - Vector3(viewUp) * nearHeight + Vector3(viewRight) * nearWidth;

	Vector3 farTopLeft     = farCenter + Vector3(viewUp) * farHeight - Vector3(viewRight) * farWidth;
	Vector3 farTopRight    = farCenter + Vector3(viewUp) * farHeight + Vector3(viewRight) * farWidth;
	Vector3 farBottomLeft  = farCenter - Vector3(viewUp) * farHeight - Vector3(viewRight) * farWidth;
	Vector3 farBottomRight = farCenter - Vector3(viewUp) * farHeight + Vector3(viewRight) * farWidth;

	frustum.topPlane = DirectX::XMPlaneFromPoints(nearTopRight, nearTopLeft, farTopLeft);
	frustum.bottomPlane = DirectX::XMPlaneFromPoints(nearBottomLeft, nearBottomRight, farBottomRight);
	frustum.leftPlane = DirectX::XMPlaneFromPoints(nearTopLeft, nearBottomLeft, farBottomLeft);
	frustum.rightPlane = DirectX::XMPlaneFromPoints(nearBottomRight, nearTopRight, farBottomRight);

	return frustum;
}
