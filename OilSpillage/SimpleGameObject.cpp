#include "SimpleGameObject.h"

SimpleGameObject::SimpleGameObject()
{
}

SimpleGameObject::~SimpleGameObject()
{
}

SimpleGameObject::SimpleGameObject(const SimpleGameObject& obj)
{
	this->color = obj.color;
	this->material = obj.material;
	this->mesh = obj.mesh;
	this->sunShadow = obj.sunShadow;
	this->spotShadow = obj.spotShadow;
	this->rigidBody = obj.rigidBody;
	this->scale = obj.scale;
}

bool SimpleGameObject::getSpotShadow() const
{
	return this->spotShadow;
}

bool SimpleGameObject::getSunShadow() const
{
	return this->sunShadow;
}

Matrix SimpleGameObject::getTransform()
{
	btQuaternion rot = rigidBody->getWorldTransform().getRotation();

	Matrix transform(Matrix::CreateScale(scale));
	transform *= Matrix::CreateFromQuaternion(Quaternion(rot.x(), rot.y(), rot.z(), rot.w()));
	transform *= Matrix::CreateTranslation(btTransformGetPosition(rigidBody->getWorldTransform()));

	return transform;
}

void SimpleGameObject::setSunShadow(bool arg)
{
	this->sunShadow = arg;
}

void SimpleGameObject::setSpotShadow(bool arg)
{
	this->spotShadow = arg;
}

void SimpleGameObject::setPosition(Vector3 newPos)
{
	if (this->rigidBody)
	{
		this->rigidBody->getWorldTransform().setOrigin(btVector3(newPos.x, newPos.y, newPos.z));
	}
}

void SimpleGameObject::setRotation(Vector3 newRotation)
{
	if (this->rigidBody)
	{
		Quaternion qt = Quaternion::CreateFromYawPitchRoll(newRotation.y, newRotation.x, newRotation.z);
		this->rigidBody->getWorldTransform().setRotation(btQuaternion(qt.x, qt.y, qt.z, qt.w));
	}
}

void SimpleGameObject::setScale(Vector3 newScale)
{
	this->scale = newScale;
}

Texture* SimpleGameObject::getTexture()
{
	return this->material.diffuse;
}

void SimpleGameObject::setTexture(Texture* diffuseTexture)
{
	this->material.diffuse = diffuseTexture;
}

Texture* SimpleGameObject::getNormalMap()
{
	return this->material.normal;
}

void SimpleGameObject::setNormalMap(Texture* normalMap)
{
	this->material.normal = normalMap;
}

Texture* SimpleGameObject::getSpecularMap()
{
	return this->material.specular;
}

void SimpleGameObject::setSpecularMap(Texture* specularMap)
{
	this->material.specular = specularMap;
}

Texture* SimpleGameObject::getGlossMap()
{
	return this->material.gloss;
}

void SimpleGameObject::setGlossMap(Texture* glossMap)
{
	this->material.gloss = glossMap;
}

Material SimpleGameObject::getMaterial()
{
	return material;
}

void SimpleGameObject::setMaterial(Material mat)
{
	this->material = mat;
}

void SimpleGameObject::setColor(Vector4 aColor)
{
	this->color = aColor;
}

Vector4 SimpleGameObject::getColor() const
{
	return this->color;
}

Vector3 SimpleGameObject::getPosition()
{
	if (this->rigidBody)
	{
		return btTransformGetPosition(rigidBody->getWorldTransform());
	}
}

Vector3 SimpleGameObject::getRotation()
{
	if (this->rigidBody)
	{
		return btTransformGetRotation(rigidBody->getWorldTransform());
	}
}

Vector3 SimpleGameObject::getScale() const
{
	return this->scale;
}

AABB& SimpleGameObject::getAABB()
{
	AABB boundingBox = this->mesh->getAABB().scale(this->scale);
	Vector3 position = this->getPosition();
	boundingBox.maxPos += position;
	boundingBox.minPos += position;
	return boundingBox;
}

btRigidBody* SimpleGameObject::getRigidBody() const
{
	return this->rigidBody;
}

void SimpleGameObject::setRigidBody(btRigidBody* body)
{
	this->rigidBody = body;
}

Vector3 SimpleGameObject::btTransformGetRotation(btTransform const& trans) const
{
	btQuaternion qt = trans.getRotation();

	return Vector3(
		std::atan2(2 * qt.getX() * qt.getW() + 2 * qt.getY() * qt.getZ(), 1 - 2 * qt.getX() * qt.getX() - 2 * qt.getZ() * qt.getZ()),
		std::atan2(2 * qt.getY() * qt.getW() + 2 * qt.getX() * qt.getZ(), 1 - 2 * qt.getY() * qt.getY() - 2 * qt.getZ() * qt.getZ()),
		std::asin(-2 * qt.getX() * qt.getY() - qt.getZ() * qt.getW())
	);
}

Vector3 SimpleGameObject::btTransformGetPosition(btTransform const& trans) const
{
	btVector3 const Position = trans.getOrigin();
	return Vector3(Position.getX(), Position.getY(), Position.getZ());
}
