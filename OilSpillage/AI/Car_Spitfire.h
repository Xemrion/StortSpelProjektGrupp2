#pragma once
#include "DynamicActor.h"
class Spitfire : public DynamicActor
{
private:
	btScalar mRotation;
	GameObject* wheel1;
	GameObject* wheel2;
	GameObject* wheel3;
	GameObject* wheel4;

	float targetRotation;
	float rotateAcceleration;
	float reverseTimer;
	float reverseTimer2;
	float soundTimer;
	float trailTimer;

	float velocitySpeed;
	class Physics* physics;

	Vector3 direction;
	float throttleInputStrength;
	void updateVehicle(); 
protected:
	btPoint2PointConstraint* pointJoint;
	GameObject* vehicleBody1;
public:
	Spitfire(float x, float z,Physics* physics);
	Spitfire();
	virtual ~Spitfire();

	void init(Physics* physics);
	void vehicleMovement(float deltaTime, float throttleInputStrength, bool throttleInputTrigger, bool reverseInputTrigger, Vector2 directionInput);

	void setAccelForce(Vector3 accelForce, float deltaTime);
	void setWheelRotation();

	float getYaw(DirectX::XMVECTOR Quaternion);
	float getHeading(Quaternion qt);
	GameObject* getVehicleBody1();

	void move();
	virtual void update(float dt, const Vector3& targetPos);
	void followPath();
};