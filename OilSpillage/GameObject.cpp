#include"GameObject.h"
#include"game.h"
GameObject::~GameObject()
{
	/*if (rigidBody != nullptr) {
		physics->DeleteRigidBody(rigidBody);
		this->rigidBody = nullptr;
	}*/
}
void GameObject::updateRigidBody()
{
}
Matrix GameObject::getTransform() {


	if (rigidBody == nullptr) {
		Matrix transform(Matrix::CreateScale(scale));
		transform *= Matrix::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
		transform *= Matrix::CreateTranslation(position);

		if (this->parent != nullptr)
		{
			transform *= this->parent->getTransform();
		}

		return transform;
	}else {

		//btVector3 pos = btVector3(rigidBody->getWorldTransform().getOrigin());
		//this->position = Vector3(pos.getX(),pos.getY(),pos.getZ());

		Matrix transform(Matrix::CreateScale(scale));

		position = btTransform_to_XMFLOAT3(rigidBody->getWorldTransform());
		btQuaternion rot = rigidBody->getWorldTransform().getRotation();
		Quaternion d3drotation(rot.x(), rot.y(), rot.z(), rot.w());

		this->rotationQt = d3drotation;

		transform *= Matrix::CreateFromQuaternion(d3drotation);
		transform *= Matrix::CreateTranslation(position);

		if (this->parent != nullptr)
		{
			transform *= this->parent->getTransform();
		}

		return transform;
	}
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

Texture* GameObject::getNormalMap()
{
	return this->normalMap;
}

void GameObject::setNormalMap(Texture* normalMap)
{
	this->normalMap = normalMap;
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
Quaternion GameObject::getRotationQuaternion() const
{
	return this->rotationQt;
}
Vector3 GameObject::getScale() const
{
	return this->scale;
}

AABB GameObject::getAABB() const
{
	AABB boundingBox = this->mesh->getAABB().scale(this->scale);
	boundingBox.maxPos += this->getPosition();
	boundingBox.minPos += this->getPosition();
	return boundingBox;
}

btRigidBody* GameObject::getRigidBody()
{
	return this->rigidBody;
}

void GameObject::setRigidBody(btRigidBody* body, Physics* physics)
{
	this->rigidBody = body;
	this->physics = physics;
}

Matrix GameObject::btTransform_to_XMMATRIX(btTransform const& trans)
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
}

Vector3 GameObject::btTransform_to_XMFLOAT3(btTransform const& trans)
{
	btVector3 const Position = trans.getOrigin();

	return Vector3(Position.getX(), Position.getY(), Position.getZ());
}
