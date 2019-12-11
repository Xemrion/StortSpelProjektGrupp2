#pragma once
#include "SimpleGameObject.h"

class GameObject : public SimpleGameObject
{
private:
	bool player = false;
protected:
	Vector3  position{ 1.0f, 1.0f, 1.0f };
	Vector3  rotation;
	Vector3 velocity;
public:
	GameObject();
	virtual ~GameObject();

	GameObject(const GameObject& obj);
	
	bool	 isPlayer() const;
	void	 makePlayer();
	Matrix   getTransform();
	void     setPosition(Vector3);
	void     setGameObjectPos(Vector3);
	void     move(Vector3 offset);
	void     setRotation(Vector3);
	Vector3 getPosition();
	Vector3	 getRotation();
	Quaternion getRotationQuaternion() const;
	void setVelocity(Vector3 velocity);
	void updateObject(float deltaTime);
};
