#pragma once
#include "DynamicActor.h"
class Spitfire : public DynamicActor
{
private:
	//GameObject* vehicle;
	btScalar mRotation;
	GameObject* wheel1;
	GameObject* wheel2;
	GameObject* wheel3;
	GameObject* wheel4;

	GameObject* vehicleBody1;

	DirectX::XMFLOAT2 velocity;

	float targetRotation;
	float rotateAcceleration;
	bool rotationDirection;
	float reverseTimer;
	float reverseTimer2;
	float soundTimer;


	float velocitySpeed;
	class Physics* physics;

	btPoint2PointConstraint* pointJoint;
	void setUpActor();

	Vector3 direction;
	float throttleInputStrength;
	Vector3 accelForce;
	void updateVehicle(); 
protected:
public:

	Spitfire(float x, float z,Physics* physics);
	Spitfire();
	virtual ~Spitfire();
	virtual btPoint2PointConstraint* getPointJoint();

	void init(Physics* physics);
	void vehicleMovement(float deltaTime, float throttleInputStrength, bool throttleInputTrigger, bool reverseInputTrigger, Vector2 directionInput);

	void setAccelForce(Vector3 accelForce, float deltaTime);
	void setWheelRotation();
	//void onCollision(Vector2 direction);


	float getYaw(DirectX::XMVECTOR Quaternion);
	float getHeading(Quaternion qt);

	void move();
	virtual void update(float dt, const Vector3& targetPos);
	void followPath();
};