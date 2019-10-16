#pragma once

// TODO: implement Ctors & OPs

#include "Mesh.h"
#include <d3d11.h>
#include "Texture.h"
#include "Physics.h"
#include <math.h>

using namespace DirectX::SimpleMath; // TODO: refactor into functions

class GameObject {
	Texture       *texture    { nullptr };
	Texture       *normalMap  { nullptr };
	GameObject    *parent     { nullptr };
	btRigidBody   *rigidBody  { nullptr }; // TODO: make unique?
	class Physics *physics;
protected:
   Vector4        color      {  .0f,  .0f,  .0f,  1.0f };
	Vector3        position   {  .0f,  .0f,  .0f };
	Vector3        scale      { 1.0f, 1.0f, 1.0f };
   Vector3        rotation   {  .0f,  .0f,  .0f };
   Quaternion     rotationQt {}; // TODO: migrate into only using quaternions
public:
	const Mesh    *mesh       { nullptr }; // TODO: refactor
	                 ~GameObject() noexcept;
	                  GameObject()                               = default;
	                  GameObject( GameObject const &  )          = default;
	                  GameObject( GameObject       && ) noexcept = default;
	GameObject       &operator=(  GameObject const &  )          = default;
	GameObject       &operator=(  GameObject       && ) noexcept = default;
	Matrix            getTransform();
	void              updateRigidBody();
	void              setPosition(  Vector3 const & );
	void              move(         Vector3 const & );
	void              addRotation(  Vector3 const & );
	void              setRotation(  Vector3 const & );
	void              setScale(     Vector3 const & );
	void              setColor(     Vector4 const & );
	void              setNormalMap( Texture * );
	void              setTexture(   Texture * );
	void              setRigidBody( btRigidBody *, Physics * );
	Vector3    const &getPosition()           const;
	Vector3    const &getRotation()           const;
	Quaternion const &getRotationQuaternion() const;
	Vector3    const &getScale()              const;
	Vector4    const &getColor()              const;
	AABB              getAABB()               const;
	Texture          *getTexture();   // TODO: return by reference
	Texture          *getNormalMap(); // TODO: return by reference
	btRigidBody      *getRigidBody(); // TODO: return by reference
	// Get Position from btTransform
	Vector3           btTransform_to_XMFLOAT3( btTransform const & );
	Matrix            btTransform_to_XMMATRIX( btTransform const & );
};
