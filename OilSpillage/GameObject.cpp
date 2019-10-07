#include"GameObject.h"
#include"game.h"
Matrix GameObject::getTransform() {

	Matrix transform(Matrix::CreateScale(scale));
	transform *= Matrix::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
	transform *= Matrix::CreateTranslation(position);

	if (this->parent != nullptr)
	{
		transform *= this->parent->getTransform();
	}

	return transform;
}

void GameObject::setPosition(Vector3 newPos)
{
	this->position = newPos;
}

void GameObject::move(Vector3 addPos)
{
	this->position += addPos;
}

void GameObject::addRotation(Vector3 addRotaiton)
{
	this->rotation += addRotaiton;
	this->rotation.x = fmod(this->rotation.x, 2 * DirectX::XM_PI);
	this->rotation.y = fmod(this->rotation.y, 2 * DirectX::XM_PI);
	this->rotation.z = fmod(this->rotation.z, 2 * DirectX::XM_PI);
}

void GameObject::setRotation(Vector3 newRotation)
{
	this->rotation = newRotation;
	this->rotation.x = fmod(this->rotation.x, 2 * DirectX::XM_PI);
	this->rotation.y = fmod(this->rotation.y, 2 * DirectX::XM_PI);
	this->rotation.z = fmod(this->rotation.z, 2 * DirectX::XM_PI);
}

void GameObject::setScale(Vector3 newScale)
{
	this->scale = newScale;
}

Texture* GameObject::getTexture()
{
	return this->texture;
}

void GameObject::setTexture(Texture* aTexture)
{
	this->texture = aTexture;
}

void GameObject::setColor(Vector4 aColor)
{
	this->color = aColor;
}

Vector4 GameObject::getColor()const
{
	return this->color;
}
Vector3 GameObject::getPosition() const
{
	return this->position;
}
Vector3 & GameObject::getPosition()
{
	return this->position;
}
Vector3 GameObject::getRotation() const
{
	return this->rotation;
}
Vector3 GameObject::getScale() const
{
	return this->scale;
}
AABB GameObject::getAABB() const
{
	return this->mesh->getAABB().scale(this->scale);
}
