#include"GameObject.h"
#include"game.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
}

GameObject::GameObject(const GameObject& obj) : GameObjectBase(obj)
{
	this->position = obj.position;
	this->rotation = obj.rotation;
	this->velocity = obj.velocity;
}

Matrix GameObject::getTransform()
{
	Matrix transform(Matrix::CreateScale(scale));
	transform *= Matrix::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
	transform *= Matrix::CreateTranslation(position);

	return transform;
}

void GameObject::setPosition(Vector3 newPos)
{
	this->position = newPos;
}

void GameObject::setGameObjectPos(Vector3 newPos)
{
	this->position = newPos;
}

void GameObject::move(Vector3 addPos)
{
	this->position += addPos;
}

void GameObject::setRotation(Vector3 newRotation)
{
	this->rotation = newRotation;
	this->rotation.x = fmod(this->rotation.x, 2 * DirectX::XM_PI);
	this->rotation.y = fmod(this->rotation.y, 2 * DirectX::XM_PI);
	this->rotation.z = fmod(this->rotation.z, 2 * DirectX::XM_PI);
}

Vector3 GameObject::getPosition() 
{
	return this->position;
}

Vector3 GameObject::getRotation() 
{
	return this->rotation;
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
