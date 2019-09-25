#include "DynamicCamera.h"
#include <cmath>

inline void DynamicCamera::updateRotationMatrix()
{
	this->rotationRes  = Matrix::CreateRotationZ(this->rotation.z); //Pitch
	this->rotationRes *= Matrix::CreateRotationX(this->rotation.x); //Roll
	this->rotationRes *= Matrix::CreateRotationY(this->rotation.y); //Yaw
}

inline void DynamicCamera::updateViewMatrix()
{
	this->view = Matrix::CreateLookAt(
		this->position, //Position in world
		this->position + Vector3::Transform(Vector3(0.0f, 0.0f, -1.0f), this->rotationRes), //Front
		Vector3::Transform(Vector3(0.0f, 1.0f, 0.0f), this->rotationRes) //Up
	);
}

DynamicCamera::DynamicCamera(float fovDegrees, float nearPlane, float farPlane)
{
	float fieldOfView = fovDegrees * (DirectX::XM_PI / 180);
	this->projection = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, 16.f / 9.f, nearPlane, farPlane);

	this->cinematicActive = false;
	this->currentObjective = 0;
	this->points = nullptr;
	this->currentSubTime = 0.0f;

	this->updateView = true;
	this->updateRotation = true;
}

DynamicCamera::DynamicCamera(Vector3 position, Vector3 rotation, float fovDegrees, float nearPlane, float farPlane)
{
	float fieldOfView = fovDegrees * (DirectX::XM_PI / 180);
	this->projection = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, 16.f / 9.f, nearPlane, farPlane);

	this->cinematicActive = false;
	this->currentObjective = 0;
	this->points = nullptr;
	this->currentSubTime = 0.0f;
	this->position = position;
	this->rotation = rotation;

	this->updateView = true;
	this->updateRotation = true;
}

DynamicCamera::~DynamicCamera()
{
}

void DynamicCamera::update(float deltaTime)
{
	if (this->cinematicActive)
	{
		CinematicPos& objective = this->points->at(this->currentObjective);
		float tempDelta = deltaTime;

		while (this->currentObjective < this->points->size() && tempDelta > objective.time - this->currentSubTime)
		{
			tempDelta -= objective.time - this->currentSubTime;

			this->currentSubTime = 0.0f;
			this->currentObjective++;
		}
		
		if (this->currentObjective < this->points->size())
		{
			CinematicPos& old = this->points->at(static_cast<size_t>(this->currentObjective) - 1);
			objective =			this->points->at(this->currentObjective);

			this->position = Vector3::Lerp(old.position, objective.position, (this->currentSubTime + tempDelta) / objective.time);
			this->rotation = Vector3::Lerp(old.rotation, objective.rotation, (this->currentSubTime + tempDelta) / objective.time);
			this->currentSubTime += tempDelta;

			this->updateView = true;
			this->updateRotation = true;
		}
		else
		{
			CinematicPos& end = this->points->back();

			this->position = end.position;
			this->rotation = end.rotation;
			this->cinematicActive = false;
		}
	}
}

void DynamicCamera::startCinematic(std::vector<CinematicPos>* points, bool startAtCurrentPos)
{
	this->cinematicActive = true;
	this->currentObjective = 1;
	this->points = points;
	this->currentSubTime = 0.0f;

	if (startAtCurrentPos)
	{
		CinematicPos startPos = { this->position, this->rotation, 0.0f };
		this->points->insert(this->points->begin(), startPos);
	}
	else
	{
		this->position = points->at(0).position;
		this->rotation = points->at(0).rotation;

		this->updateView = true;
		this->updateRotation = true;
	}
}

void DynamicCamera::stopCinematic()
{
	this->cinematicActive = false;
}

bool DynamicCamera::isCinematicDone()
{
	return this->cinematicActive;
}

Vector3 DynamicCamera::getPosition()
{
	return this->position;
}

const Vector3& DynamicCamera::getPositionRef()
{
	return this->position;
}

void DynamicCamera::setPosition(Vector3 position)
{
	if (!this->cinematicActive)
	{
		this->position = position;
		this->updateView = true;
	}
}

Vector3 DynamicCamera::getRotation()
{
	return this->rotation;
}

const Vector3& DynamicCamera::getRotationRef()
{
	return this->rotation;
}

void DynamicCamera::setRotation(Vector3 rotation)
{
	if (!this->cinematicActive)
	{
		this->rotation = rotation;
		this->updateView = true;
		this->updateRotation = true;
	}
}

Matrix DynamicCamera::getViewMatrix()
{
	if (this->updateRotation) this->updateRotationMatrix();
	if (this->updateView)     this->updateViewMatrix();

	return this->view;
}

Matrix DynamicCamera::getProjectionMatrix()
{
	return this->projection;
}
