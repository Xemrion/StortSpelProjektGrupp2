#include "DynamicGameObject.h"

DynamicGameObject::DynamicGameObject()
{
}

DynamicGameObject::~DynamicGameObject()
{
}

DynamicGameObject::DynamicGameObject(const DynamicGameObject& obj) : GameObjectBase(obj)
{
	this->rigidBody = obj.rigidBody;
	this->player = obj.player;
}

bool DynamicGameObject::isPlayer() const
{
	return this->player;
}

void DynamicGameObject::makePlayer()
{
	this->player = true;
}

Matrix DynamicGameObject::getTransform()
{
	assert(this->rigidBody && "No rigidbody has been set yet!");
	btQuaternion rot = rigidBody->getWorldTransform().getRotation();

	Matrix transform(Matrix::CreateScale(scale));
	transform *= Matrix::CreateFromQuaternion(Quaternion(rot.x(), rot.y(), rot.z(), rot.w()));
	transform *= Matrix::CreateTranslation(btTransformGetPosition(rigidBody->getWorldTransform()));

	return transform;
}

void DynamicGameObject::setPosition(Vector3 newPos)
{
	if (this->rigidBody)
	{
		this->rigidBody->getWorldTransform().setOrigin(btVector3(newPos.x, newPos.y, newPos.z));
	}
}

void DynamicGameObject::setRotation(Vector3 newRotation)
{
	if (this->rigidBody)
	{
		Quaternion qt = Quaternion::CreateFromYawPitchRoll(newRotation.y, newRotation.x, newRotation.z);
		this->rigidBody->getWorldTransform().setRotation(btQuaternion(qt.x, qt.y, qt.z, qt.w));
	}
}

Vector3 DynamicGameObject::getPosition()
{
	if (this->rigidBody)
	{
		return btTransformGetPosition(rigidBody->getWorldTransform());
	}

	assert(this->rigidBody && "No rigidbody has been set yet!");
	return Vector3();
}

Vector3 DynamicGameObject::getRotation()
{
	if (this->rigidBody)
	{
		return btTransformGetRotation(rigidBody->getWorldTransform());
	}

	assert(this->rigidBody && "No rigidbody has been set yet!");
	return Vector3();
}

Quaternion DynamicGameObject::getRotationQuaternion() const
{
	if (this->rigidBody)
	{
		btQuaternion quaternion = rigidBody->getWorldTransform().getRotation();
		return Quaternion(quaternion.getX(), quaternion.getY(), quaternion.getZ(), quaternion.getW());
	}

	assert(this->rigidBody && "No rigidbody has been set yet!");
	return Quaternion();
}

btRigidBody* DynamicGameObject::getRigidBody() const
{
	return this->rigidBody;
}

void DynamicGameObject::setRigidBody(btRigidBody* body)
{
	this->rigidBody = body;
}