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
	bool shading = true;
	bool player = false;
protected:
	Vector3  position{ 1.0f, 1.0f, 1.0f };
	Vector3  scale{ 1.0f, 1.0f, 1.0f };
	Vector3  rotation;
	Vector4  color;
	Material material;
	Vector3 velocity;
public:
	GameObject();
	~GameObject();

	GameObject(const GameObject& obj);

	const Mesh *mesh   = nullptr;
	
	bool getShading()const;
	bool getSpotShadow()const;
	bool getSunShadow()const;
	bool isPlayer() const;
	void makePlayer();
	Matrix   getTransform();
	void setShading(bool arg);
	void setSunShadow(bool arg);
	void setSpotShadow(bool arg);
	void     setPosition(const Vector3&);
	void     setGameObjectPos(Vector3);
	void     move(Vector3 offset);
	void     addRotation(const Vector3&);
	void     setRotation(const Vector3&);
	void     setScale(const Vector3&);
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
	const Vector3 & getPosition();
	const Vector3& getPositionRef();
	Vector3  getRotation();
	Quaternion getRotationQuaternion() const;
	Vector3  getScale()    const;
	AABB&     getAABB();
	btRigidBody* getRigidBody() const;
	void setRigidBody(btRigidBody* body, Physics* physics);
	void setVelocity(Vector3 velocity);
	void updateObject(float deltaTime);

	//Matrix btTransformToMatrix(btTransform const& trans) const;
	Vector3 btTransformGetRotation(btTransform const& trans) const;
	Vector3 btTransformGetPosition(btTransform const& trans) const;
};
