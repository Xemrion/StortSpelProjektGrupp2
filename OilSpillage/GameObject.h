#pragma once
#include "Mesh.h"
#include <d3d11.h>
#include "Texture.h"
#include "Physics.h"
#include <math.h>

using namespace DirectX::SimpleMath;

class GameObject
{
private:
	btRigidBody* rigidBody = nullptr;
	Physics* physics = nullptr;
	bool spotShadow = true;
	bool sunShadow = true;
protected:
	Vector3  position{ 1.0f, 1.0f, 1.0f };
	Vector3  scale{ 1.0f, 1.0f, 1.0f };
	Vector3  rotation;
	Quaternion rotationQt;
	Vector4  color;
	Material material;
	Vector3 velocity;
public:
	~GameObject();

	const Mesh* mesh = nullptr;
	GameObject* parent = nullptr;


	bool getSpotShadow()const;
	bool getSunShadow()const;
	Matrix   getTransform();
	void setSunShadow(bool arg);
	void setSpotShadow(bool arg);
	void     setPosition(Vector3);
	void     move(Vector3 offset);
	void     addRotation(Vector3);
	void     setRotation(Vector3);
	void     setScale(Vector3);
	Texture* getTexture();
	void     setTexture(Texture*);
	Texture* getNormalMap();
	void     setNormalMap(Texture*);
	Texture* getSpecularMap();
	void setSpecularMap(Texture*);
	Texture* getGlossMap();
	void setGlossMap(Texture*);
	Material getMaterial();
	void setMaterial(Material);
	void     setColor(Vector4 aColor);
	Vector4  getColor()    const;
	Vector3  getPosition();
	const Vector3& getPositionRef();
	Vector3  getRotation();
	Quaternion getRotationQuaternion() const;
	Vector3  getScale()    const;
	AABB     getAABB();
	btRigidBody* getRigidBody() const;
	void setRigidBody(btRigidBody* body, Physics* physics);
	void setVelocity(Vector3 velocity);
	void updateObject(float deltaTime);

	//Matrix btTransformToMatrix(btTransform const& trans) const;
	Vector3 btTransformGetRotation(btTransform const& trans) const;
	Vector3 btTransformGetPosition(btTransform const& trans) const;
};
