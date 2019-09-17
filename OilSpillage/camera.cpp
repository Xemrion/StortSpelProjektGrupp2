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


Matrix Camera::getViewMatrix()
{
	return XMMatrixLookAtLH(pos, pos + Vector3(0.0, -1.0, 0.0), Vector3(0.0, 0.0, 1.0));
}
