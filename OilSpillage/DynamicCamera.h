#ifndef DYNAMIC_CAMERA_H
#define DYNAMIC_CAMERA_H

#include <d3d11.h>
#include <SimpleMath.h>
#include <Vector>
#include "Structs.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

struct Frustum {
	Vector4 nearPlane;
	Vector4 farPlane;
	Vector4 leftPlane;
	Vector4 rightPlane;
	Vector4 topPlane;
	Vector4 bottomPlane;
};

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

	float fieldOfView;
	float nearDist;
	float farDist;
	float nearHeight;
	float nearWidth;
	float farHeight;
	float farWidth;

	void updateRotationMatrix();
	void updateViewMatrix();

public:
	DynamicCamera(float fovDegrees = 90.0f, float nearPlane = 1.0f, float farPlane = 1000.0f);
	DynamicCamera(Vector3 position, Vector3 rotation = Vector3(), float fovDegrees = 90.0f, float nearPlane = 1.0f, float farPlane = 1000.0f);
	~DynamicCamera();

	void update(float deltaTime);
	void startCinematic(std::vector<CinematicPos>* points, bool startAtCurrentPos);
	void stopCinematic();
	bool isCinematicDone() const;

	Vector3 getPosition() const;
	const Vector3& getPositionRef() const;
	void setPosition(Vector3 position);
	Vector3 getRotation() const;
	const Vector3& getRotationRef() const;
	void setRotation(Vector3 rotation);

	Matrix getViewMatrix();
	Matrix getProjectionMatrix() const;
	Frustum getFrustum();
};

#endif // !DYNAMIC_CAMERA_H