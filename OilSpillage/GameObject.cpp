#include"GameObject.h"
#include"game.h"

GameObject::~GameObject()
{
	/*if (rigidBody != nullptr) {
		physics->DeleteRigidBody(rigidBody);
		this->rigidBody = nullptr;
	}*/
}

Matrix GameObject::getTransform()
{
	if (this->rigidBody)
	{
		this->position = btTransformGetPosition(rigidBody->getWorldTransform());
		this->rotation = btTransformGetRotation(rigidBody->getWorldTransform());
	}

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

void GameObject::addRotation(Vector3 addRotaiton)
{
	if (this->rigidBody)
	{
		Vector3 newRotation = btTransformGetRotation(this->rigidBody->getWorldTransform());

		newRotation += addRotaiton;
		newRotation.x = fmod(newRotation.x, 2 * DirectX::XM_PI);
		newRotation.y = fmod(newRotation.y, 2 * DirectX::XM_PI);
		newRotation.z = fmod(newRotation.z, 2 * DirectX::XM_PI);

		Quaternion qt = Quaternion::CreateFromYawPitchRoll(newRotation.y, newRotation.x, newRotation.z);
		this->rigidBody->getWorldTransform().setRotation(btQuaternion(qt.x, qt.y, qt.z, qt.w));
		this->rotation = newRotation;
	}
	else
	{
		this->rotation += addRotaiton;
		this->rotation.x = fmod(this->rotation.x, 2 * DirectX::XM_PI);
		this->rotation.y = fmod(this->rotation.y, 2 * DirectX::XM_PI);
		this->rotation.z = fmod(this->rotation.z, 2 * DirectX::XM_PI);
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

void GameObject::setScale(Vector3 newScale)
{
	this->scale = newScale;
}

Texture* GameObject::getTexture()
{
	return this->material.diffuse;
}

void GameObject::setTexture(Texture* diffuseTexture)
{
	this->material.diffuse = diffuseTexture;
}

Texture* GameObject::getNormalMap()
{
	return this->material.normal;
}

void GameObject::setNormalMap(Texture* normalMap)
{
	this->material.normal = normalMap;
}

Texture* GameObject::getSpecularMap()
{
	return this->material.specular;
}

void GameObject::setSpecularMap(Texture* specularMap)
{
	this->material.specular = specularMap;
}

Texture* GameObject::getGlossMap()
{
	return this->material.gloss;
}

void GameObject::setGlossMap(Texture* glossMap)
{
	this->material.gloss = glossMap;
}

Material GameObject::getMaterial()
{
	return material;
}

void GameObject::setMaterial(Material mat)
{
	this->material = mat;
}


void GameObject::setColor(Vector4 aColor)
{
	this->color = aColor;
}

Vector4 GameObject::getColor()const
{
	return this->color;
}

Vector3 GameObject::getPosition()
{
	if (this->rigidBody)
	{
		this->position = btTransformGetPosition(rigidBody->getWorldTransform());
	}

	return this->position;
}

const Vector3 & GameObject::getPositionRef()
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
Vector3 GameObject::getScale() const
{
	return this->scale;
}

AABB GameObject::getAABB()
{
	AABB boundingBox = this->mesh->getAABB().scale(this->scale);
	boundingBox.maxPos += this->getPosition();
	boundingBox.minPos += this->getPosition();
	return boundingBox;
}

btRigidBody* GameObject::getRigidBody() const
{
	return this->rigidBody;
}

void GameObject::setRigidBody(btRigidBody* body, Physics* physics)
{
	this->rigidBody = body;
	this->physics = physics;
}

/*Matrix GameObject::btTransformToMatrix(btTransform const& trans) const
{
	//store btTranform in 4x4 Matrix
	Matrix matrix;
	//XMFLOAT4X4 matrix4x4 = XMFLOAT4X4();
	btMatrix3x3 const& Rotation = trans.getBasis();
	btVector3 const& Position = trans.getOrigin();
	// copy rotation matrix
	for (int row = 0; row < 3; ++row)
	{

		for (int column = 0; column < 3; ++column)
		{
			matrix.m[row][column] = Rotation[column][row];
		}
	}

	// copy position
	for (int column = 0; column < 3; ++column)
	{
		matrix.m[3][column] = Position[column];
	}
	return matrix;
}*/

Vector3 GameObject::btTransformGetRotation(btTransform const& trans) const
{
	btQuaternion qt = trans.getRotation();

	return Vector3(
		std::atan2(2 * qt.getX() * qt.getW() + 2 * qt.getY() * qt.getZ(), 1 - 2 * qt.getX() * qt.getX() - 2 * qt.getZ() * qt.getZ()),
		std::atan2(2 * qt.getY() * qt.getW() + 2 * qt.getX() * qt.getZ(), 1 - 2 * qt.getY() * qt.getY() - 2 * qt.getZ() * qt.getZ()),
		std::asin(2 * qt.getX() * qt.getY() + 2 * qt.getZ() * qt.getW())
	);
}

Vector3 GameObject::btTransformGetPosition(btTransform const& trans) const
{
	btVector3 const Position = trans.getOrigin();
	return Vector3(Position.getX(), Position.getY(), Position.getZ());
}
