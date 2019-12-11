#pragma once
#include "Mesh.h"
#include <d3d11.h>
#include "Texture.h"
#include "Physics.h"
#include <math.h>

using namespace DirectX::SimpleMath;

class SimpleGameObject
{
private:
	bool spotShadow = true;
	bool sunShadow = true;
protected:
	btRigidBody* rigidBody = nullptr;
	Vector3  scale{ 1.0f, 1.0f, 1.0f };
	Vector4  color;
	Material material;
public:
	SimpleGameObject();
	virtual ~SimpleGameObject();

	SimpleGameObject(const SimpleGameObject& obj);

	const Mesh* mesh = nullptr;

	bool		 getSpotShadow()const;
	bool		 getSunShadow()const;
	virtual Matrix getTransform();
	void		 setSunShadow(bool arg);
	void		 setSpotShadow(bool arg);
	virtual void setPosition(Vector3);
	virtual void setRotation(Vector3);
	void		 setScale(Vector3);
	Texture*	 getTexture();
	void		 setTexture(Texture*);
	Texture*	 getNormalMap();
	void		 setNormalMap(Texture*);
	Texture*	 getSpecularMap();
	void		 setSpecularMap(Texture*);
	Texture*	 getGlossMap();
	void		 setGlossMap(Texture*);
	Material	 getMaterial();
	void		 setMaterial(Material);
	void		 setColor(Vector4 aColor);
	Vector4		 getColor()    const;
	virtual Vector3	getPosition();
	virtual Vector3	getRotation();
	Vector3		 getScale()    const;
	AABB&		 getAABB();
	btRigidBody* getRigidBody() const;
	void		 setRigidBody(btRigidBody* body);
	
	Vector3		 btTransformGetRotation(btTransform const& trans) const;
	Vector3		 btTransformGetPosition(btTransform const& trans) const;
};
