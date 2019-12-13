#include "GameObjectBase.h"

GameObjectBase::GameObjectBase()
{
}

GameObjectBase::~GameObjectBase()
{
}

GameObjectBase::GameObjectBase(const GameObjectBase& obj)
{
	this->color = obj.color;
	this->material = obj.material;
	this->mesh = obj.mesh;
	this->sunShadow = obj.sunShadow;
	this->spotShadow = obj.spotShadow;
	this->scale = obj.scale;
}

bool GameObjectBase::getSpotShadow() const
{
	return this->spotShadow;
}

bool GameObjectBase::getSunShadow() const
{
	return this->sunShadow;
}

void GameObjectBase::setSunShadow(bool arg)
{
	this->sunShadow = arg;
}

void GameObjectBase::setSpotShadow(bool arg)
{
	this->spotShadow = arg;
}

void GameObjectBase::setScale(Vector3 scale)
{
	this->scale = scale;
}

Texture* GameObjectBase::getTexture() const
{
	return this->material.diffuse;
}

void GameObjectBase::setTexture(Texture* texture)
{
	this->material.diffuse = texture;
}

Texture* GameObjectBase::getNormalMap() const
{
	return this->material.normal;
}

void GameObjectBase::setNormalMap(Texture* normalMap)
{
	this->material.normal = normalMap;
}

Texture* GameObjectBase::getSpecularMap() const
{
	return this->material.specular;
}

void GameObjectBase::setSpecularMap(Texture* specularMap)
{
	this->material.specular = specularMap;
}

Texture* GameObjectBase::getGlossMap() const
{
	return this->material.gloss;
}

void GameObjectBase::setGlossMap(Texture* glossMap)
{
	this->material.gloss = glossMap;
}

Material GameObjectBase::getMaterial() const
{
	return material;
}

void GameObjectBase::setMaterial(const Material& material)
{
	this->material = material;
}

void GameObjectBase::setColor(const Vector4& aColor)
{
	this->color = aColor;
}

Vector4 GameObjectBase::getColor() const
{
	return this->color;
}

Vector3 GameObjectBase::getScale() const
{
	return this->scale;
}

AABB GameObjectBase::getAABB()
{
	AABB boundingBox = this->mesh->getAABB().scale(this->scale);
	Vector3 position = this->getPosition();
	boundingBox.maxPos += position;
	boundingBox.minPos += position;
	return boundingBox;
}

Vector3 GameObjectBase::btTransformGetRotation(btTransform const& trans)
{
	btQuaternion qt = trans.getRotation();

	return Vector3(
		std::atan2(2 * qt.getX() * qt.getW() + 2 * qt.getY() * qt.getZ(), 1 - 2 * qt.getX() * qt.getX() - 2 * qt.getZ() * qt.getZ()),
		std::atan2(2 * qt.getY() * qt.getW() + 2 * qt.getX() * qt.getZ(), 1 - 2 * qt.getY() * qt.getY() - 2 * qt.getZ() * qt.getZ()),
		std::asin(-2 * qt.getX() * qt.getY() - qt.getZ() * qt.getW())
	);
}

Vector3 GameObjectBase::btTransformGetPosition(btTransform const& trans)
{
	btVector3 const Position = trans.getOrigin();
	return Vector3(Position.getX(), Position.getY(), Position.getZ());
}

Vector3 GameObjectBase::btVectorConv(btVector3 const& vector)
{
	return Vector3(vector.operator const float *());
}
