#include"GameObject.h"
#include"game.h"

GameObject::~GameObject() noexcept
{
	if ( rigidBody ) {
      // TODO: refactor so UPtrs are used instead?
		physics->deleteRigidBody( rigidBody );
		rigidBody = nullptr;
	}
}

void GameObject::updateRigidBody()
{
   // TODO?
}

Matrix GameObject::getTransform()
{
	if ( !rigidBody ) {
		Matrix transform(Matrix::CreateScale(scale));
		transform *= Matrix::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z);
		transform *= Matrix::CreateTranslation(position);

		if ( parent )
		{
			transform *= parent->getTransform();
		}

		return transform;
	} else {

		//btVector3 pos = btVector3(rigidBody->getWorldTransform().getOrigin());
		//this->position = Vector3(pos.getX(),pos.getY(),pos.getZ());

		Matrix transform(Matrix::CreateScale(scale));

		position = btTransform_to_XMFLOAT3(rigidBody->getWorldTransform());
		btQuaternion rot = rigidBody->getWorldTransform().getRotation();
		Quaternion d3drotation(rot.x(), rot.y(), rot.z(), rot.w());

		rotationQt = d3drotation;

		transform *= Matrix::CreateFromQuaternion(d3drotation);
		transform *= Matrix::CreateTranslation(position);

		if ( parent )
		{
			transform *= parent->getTransform();
		}

		return transform;
	}
}

void GameObject::setPosition( Vector3 const &p )
{
	position = p;
}

void GameObject::move( Vector3 const &offset )
{
	position += offset;
}

void GameObject::addRotation( Vector3 const &r )
{
	rotation += r;
	rotation.x = fmod( rotation.x, 2 * DirectX::XM_PI );
	rotation.y = fmod( rotation.y, 2 * DirectX::XM_PI );
	rotation.z = fmod( rotation.z, 2 * DirectX::XM_PI );
}

void GameObject::setRotation( Vector3 const &r )
{
	rotation = r;
	rotation.x = fmod( rotation.x, 2 * DirectX::XM_PI );
	rotation.y = fmod( rotation.y, 2 * DirectX::XM_PI );
	rotation.z = fmod( rotation.z, 2 * DirectX::XM_PI );
}

void GameObject::setScale( Vector3 const &s )
{
	scale = s;
}

Texture* GameObject::getTexture()
{
	return texture;
}

void GameObject::setTexture( Texture *t )
{
	texture = t;
}

Texture *GameObject::getNormalMap()
{
	return normalMap;
}

void GameObject::setNormalMap( Texture *n )
{
	normalMap = n;
}

void GameObject::setColor( Vector4 const &c )
{
	color = c;
}

Vector4 const &GameObject::getColor() const
{
	return color;
}

Vector3 const &GameObject::getPosition() const
{
	return position;
}

Vector3 const &GameObject::getRotation() const
{
	return rotation;
}

Quaternion const &GameObject::getRotationQuaternion() const
{
	return rotationQt;
}

Vector3 const &GameObject::getScale() const
{
	return scale;
}

AABB GameObject::getAABB() const
{
	AABB boundingBox = this->mesh->getAABB().scale(this->scale);
   auto position = getPosition();
	boundingBox.maxPos += position;
	boundingBox.minPos += position;
	return boundingBox;
}

btRigidBody *GameObject::getRigidBody()
{
	return rigidBody;
}

void GameObject::setRigidBody( btRigidBody *b, Physics *p )
{
	rigidBody = b;
	physics   = p;
}

Matrix GameObject::btTransform_to_XMMATRIX( btTransform const &t )
{
	// store btTranform in 4x4 Matrix
	Matrix matrix;
	// XMFLOAT4X4 matrix4x4 = XMFLOAT4X4();
	btMatrix3x3 const &rot = t.getBasis();
	btVector3   const &pos = t.getOrigin();
	// copy rotation matrix
	for ( int row = 0;  row < 3;  ++row )
		for ( int column = 0;  column < 3;  ++column )
			matrix.m[row][column] = rot[column][row];

	// copy position
	for ( int column = 0;  column < 3;  ++column )
		matrix.m[3][column] = pos[column];

	return matrix;
}

Vector3 GameObject::btTransform_to_XMFLOAT3(btTransform const& trans)
{
	btVector3 const pos = trans.getOrigin();
	return Vector3( pos.getX(), pos.getY(), pos.getZ() );
}
