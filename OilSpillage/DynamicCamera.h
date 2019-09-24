#ifndef DYNAMIC_CAMERA_H
#define DYNAMIC_CAMERA_H

#include <d3d11.h>
#include <SimpleMath.h>
#include <Vector>
#include "Structs.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

class DynamicCamera
{
private:
	Matrix projection;
	Vector3 position;
	Vector3 rotation;

	Matrix view;
	Matrix rotationRes;
	bool updateView;
	bool updateRotation;

	std::vector<CinematicPos>* points;
	int currentObjective;
	bool cinematicActive;
	float currentSubTime;

	void updateRotationMatrix();
	void updateViewMatrix();

public:
	DynamicCamera(float fovDegrees = 90.0f, float nearPlane = 1.0f, float farPlane = 1000.0f);
	DynamicCamera(Vector3 position, Vector3 rotation = Vector3(), float fovDegrees = 90.0f, float nearPlane = 1.0f, float farPlane = 1000.0f);
	~DynamicCamera();

	void update(float deltaTime);
	void startCinematic(std::vector<CinematicPos>* points, bool startAtCurrentPos);
	void stopCinematic();
	bool isCinematicDone();

	Vector3 getPosition();
	const Vector3& getPositionRef();
	void setPosition(Vector3 position);
	Vector3 getRotation();
	const Vector3& getRotationRef();
	void setRotation(Vector3 rotation);

	Matrix getViewMatrix();
	Matrix getProjectionMatrix();
};

#endif // !DYNAMIC_CAMERA_H
