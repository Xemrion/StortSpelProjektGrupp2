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

	float targetRotation;
	float rotateAcceleration;
	bool rotationDirection;
	float reverseTimer;
	float reverseTimer2;
	float soundTimer;

	float trailTimer;

	float velocitySpeed;
	class Physics* physics;


	void setUpActor();
	Vector3 direction;
	float throttleInputStrength;
	Vector3 accelForce;
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
	//void onCollision(Vector2 direction);


	float getYaw(DirectX::XMVECTOR Quaternion);
	float getHeading(Quaternion qt);

	void move();
	virtual void update(float dt, const Vector3& targetPos);
	void followPath();
};