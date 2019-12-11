#include"GameObject.h"
#include"game.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

GameObject::GameObject(const GameObject& obj) : SimpleGameObject(obj)
{
	this->position = obj.position;
	this->rotation = obj.rotation;
}

Matrix GameObject::getTransform()
{
	if (this->rigidBody)
	{
		this->position = btTransformGetPosition(rigidBody->getWorldTransform());
		this->rotation = btTransformGetRotation(rigidBody->getWorldTransform());
	}

	Matrix transform(Matrix::CreateScale(scale));
	if (!this->rigidBody) {
		transform *= Matrix::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
	}
	else {
		btQuaternion rot = rigidBody->getWorldTransform().getRotation();
		Quaternion d3drotation(rot.x(), rot.y(), rot.z(), rot.w());
		
		transform *= Matrix::CreateFromQuaternion(d3drotation);
	}
	transform *= Matrix::CreateTranslation(position);

	return transform;
}

void GameObject::setPosition(Vector3 newPos)
{
	if (this->rigidBody)
	{
		this->rigidBody->getWorldTransform().setOrigin(btVector3(newPos.x, newPos.y, newPos.z));
		this->position = btTransformGetPosition(rigidBody->getWorldTransform());
	}
	else
	{
		this->position = newPos;
	}
}

void GameObject::setGameObjectPos(Vector3 newPos)
{
	this->position = newPos;

}

void GameObject::move(Vector3 addPos)
{
	if (this->rigidBody)
	{
		this->rigidBody->getWorldTransform().getOrigin() += btVector3(addPos.x, addPos.y, addPos.z);
		this->position = btTransformGetPosition(rigidBody->getWorldTransform());
	}
	else
	{
		this->position += addPos;
	}
}

void GameObject::setRotation(Vector3 newRotation)
{
	if (this->rigidBody)
	{
		Quaternion qt = Quaternion::CreateFromYawPitchRoll(newRotation.y, newRotation.x, newRotation.z);
		this->rigidBody->getWorldTransform().setRotation(btQuaternion(qt.x, qt.y, qt.z, qt.w));
		this->rotation = newRotation;
	}
	else
	{
		this->rotation = newRotation;
		this->rotation.x = fmod(this->rotation.x, 2 * DirectX::XM_PI);
		this->rotation.y = fmod(this->rotation.y, 2 * DirectX::XM_PI);
		this->rotation.z = fmod(this->rotation.z, 2 * DirectX::XM_PI);
	}
}

Vector3 GameObject::getPosition() 
{
	if (this->rigidBody)
	{
		this->position = btTransformGetPosition(rigidBody->getWorldTransform());
	}

	return this->position;
}

Vector3 GameObject::getRotation() 
{
	if (this->rigidBody)
	{
		this->rotation = btTransformGetRotation(rigidBody->getWorldTransform());
	}

	return this->rotation;
}
Quaternion GameObject::getRotationQuaternion() const
{
	if (this->rigidBody)
	{
		btQuaternion quaternion = rigidBody->getWorldTransform().getRotation();
		return Quaternion(quaternion.getX(), quaternion.getY(), quaternion.getZ(), quaternion.getW());
	}

	return XMQuaternionRotationRollPitchYaw(this->rotation.y, this->rotation.x, this->rotation.z);
}

void GameObject::setVelocity(Vector3 velocity)
{
	this->velocity = velocity;
}

void GameObject::updateObject(float deltaTime)
{
	if (this->getPosition().y > -1.4f) {
		velocity.y -= 9.82f*deltaTime;
		velocity /= 1 + (1.5f * deltaTime);
	}
	else {
		this->setPosition(Vector3(this->getPosition().x,-1.4f, this->getPosition().z));
		velocity.y = 0;
		velocity /= 1 + (4.5f * deltaTime);
	}
	this->setRotation(Vector3(velocity.y*0.12f*this->getRotation().y,this->getRotation().y, velocity.y * 0.2f * -this->getRotation().y));

	this->setPosition(this->getPosition() + this->velocity * deltaTime);
}

bool GameObject::isPlayer() const
{
	return this->player;
}

void GameObject::makePlayer()
{
	this->player = true;
}
