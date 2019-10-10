#pragma once
#include "Mesh.h"
#include <d3d11.h>
#include "Texture.h"
#include <math.h>

using namespace DirectX::SimpleMath; 
class GameObject {
protected:
   Vector3  position { 1.0f, 1.0f, 1.0f };
	Vector3  scale    { 1.0f, 1.0f, 1.0f };
	Vector3  rotation;
	Vector4  color;
	Texture *texture   = nullptr;
public:
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
	void     setColor(Vector4 aColor);
	Vector4  getColor()    const;
	Vector3  getPosition() const;
	Vector3 &getPosition();
	Vector3  getRotation() const;
	Vector3  getScale()    const;
	AABB     getAABB()     const;
};


class StaticGameObject : public GameObject, public Mesh
{
public:
	AABB getAABB() const;
};
