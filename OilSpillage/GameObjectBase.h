#pragma once
#include "Mesh.h"
#include <d3d11.h>
#include "Texture.h"
#include "Physics.h"
#include <math.h>

using namespace DirectX::SimpleMath;

class GameObjectBase
{
private:
	bool spotShadow = true;
	bool sunShadow = true;
protected:
	Vector3  scale{ 1.0f, 1.0f, 1.0f };
	Vector4  color;
	Material material;
public:
	GameObjectBase();
	virtual ~GameObjectBase();

	GameObjectBase(const GameObjectBase& obj);

	const Mesh* mesh = nullptr;

	virtual	Matrix	getTransform() = 0;
	virtual void	setPosition(Vector3) = 0;
	virtual void	setRotation(Vector3) = 0;
	virtual Vector3	getPosition() = 0;
	virtual Vector3	getRotation() = 0;

	bool			getSpotShadow() const;
	bool			getSunShadow() const;
	void			setSunShadow(bool arg);
	void			setSpotShadow(bool arg);
	void			setScale(Vector3 scale);
	Texture*		getTexture() const;
	void			setTexture(Texture* texture);
	Texture*		getNormalMap() const;
	void			setNormalMap(Texture* normalMap);
	Texture*		getSpecularMap() const;
	void			setSpecularMap(Texture* specularMap);
	Texture*		getGlossMap() const;
	void			setGlossMap(Texture* glossMap);
	Material		getMaterial() const;
	void			setMaterial(const Material& material);
	void			setColor(const Vector4& aColor);
	Vector4			getColor() const;
	Vector3			getScale() const;
	AABB			getAABB();

	static Vector3	btTransformGetRotation(btTransform const& trans);
	static Vector3	btTransformGetPosition(btTransform const& trans);
	static Vector3	btVectorConv(btVector3 const& vector);
};
