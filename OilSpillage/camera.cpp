#include "Camera.h"
#include <DirectXMath.h>

Camera::Camera()
{
	
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

float Camera::getSpeed()
{
	return this->speed;
}

void Camera::setSpeed(float newSpeed)
{
	this->speed = newSpeed;
}

Matrix Camera::getViewMatrix()
{
	view = Matrix();
	Matrix rotationX = Matrix::CreateRotationX(orientation.x);
	Matrix rotationY = Matrix::CreateRotationY(orientation.y);
	Matrix rotationZ = Matrix::CreateRotationZ(orientation.z);

	view = view * rotationX;
	view = view * rotationY;
	view = view * rotationZ;
	view = view * Matrix::CreateTranslation(-pos);

	return view;
}
