#pragma once
#include "Mesh.h"
#include <d3d11.h>
#include "Texture.h"
#include "Physics.h"
#include <math.h>

using namespace DirectX::SimpleMath; 
class GameObject {
private:
	btRigidBody* rigidBody;
	class Physics* physics;
protected:
   Vector3  position { 1.0f, 1.0f, 1.0f };
	Vector3  scale    { 1.0f, 1.0f, 1.0f };
	Vector3  rotation;
	Quaternion rotationQt;
	Vector4  color;
	Texture* texture   = nullptr;
	Texture* normalMap = nullptr;
public:
	~GameObject();
	void updateRigidBody();
	const Mesh *mesh   = nullptr;
	GameObject *parent = nullptr;
	
	Matrix   getTransform();
	void     setPosition(Vector3);
	void     move(Vector3 offset);
	void     addRotation(Vector3);
	void     setRotation(Vector3);
	void     setScale(Vector3);
	Texture *getTexture();
	void     setTexture(Texture *);
	Texture* getNormalMap();
	void     setNormalMap(Texture*);
	void     setColor(Vector4 aColor);
	Vector4  getColor()    const;
	Vector3  getPosition() const;
	Vector3 &getPosition();
	Vector3  getRotation() const;
	Quaternion getRotationQuaternion() const;
	Vector3  getScale()    const;
	AABB     getAABB()     const;
	btRigidBody* getRigidBody();
	void setRigidBody(btRigidBody* body, Physics* physics);
	Matrix btTransform_to_XMMATRIX(btTransform const& trans);
	//Get Position from btTransform
	Vector3 btTransform_to_XMFLOAT3(btTransform const& trans);
};
