#pragma once
#include "GameObjectBase.h"

class DynamicGameObject : public GameObjectBase
{
private:
	bool player = false;
	btRigidBody* rigidBody = nullptr;
public:
	DynamicGameObject();
	virtual ~DynamicGameObject();

	DynamicGameObject(const DynamicGameObject& obj);

	bool		 isPlayer() const;
	void		 makePlayer();
	Matrix		 getTransform();
	void		 setPosition(Vector3 position);
	void		 setRotation(Vector3 rotation);
	Vector3		 getPosition();
	Vector3		 getRotation();
	Quaternion	 getRotationQuaternion() const;
	btRigidBody* getRigidBody() const;
	void		 setRigidBody(btRigidBody* body);
};
