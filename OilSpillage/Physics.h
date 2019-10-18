#pragma once

#include "BulletSDK/include/btBulletDynamicsCommon.h"
#include <vector>
#include <iostream>
#include <string>
#include <DirectXMath.h>
#include <d3d11.h>
#include <SimpleMath.h>
#include <algorithm>
#include <memory>
#include "PG/defs.hpp"

class Physics
{
private:
	Unique<btDbvtBroadphase>                     broadphase      {};
	Unique<btDefaultCollisionConfiguration>      collisionConfig {};
	Unique<btCollisionDispatcher>                dispatcher      {};
	Unique<btSequentialImpulseConstraintSolver>  solver          {};
	btDiscreteDynamicsWorld                     *world           { nullptr }; // hacky, but unique_ptr bugs out here
	Vector<Unique<btSphereShape>>                spheres         {};
	Vector<Unique<btBoxShape>>                   boxes           {};
	Vector<Unique<btDefaultMotionState>>         motions         {};
	Vector<Unique<btRigidBody>>                  bodies          {};
	Vector<Unique<btCollisionShape>>             shapes          {};
	Unique<btStaticPlaneShape>                   plane           {};
public:
	Physics();
	~Physics();
	void update( float deltaTime );
	btRigidBody *addSphere( float radius, btVector3 Origin, float mass );
	btRigidBody *addBox( btVector3 Origin, btVector3 size, float mass );
	/*btRigidBody* addPlayer(btVector3 Origin, btVector3 size, float mass,Player *player);*/
	void deleteRigidBody( btRigidBody * );
	//static bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2);
	//void renderSphere(btRigidBody* sphere);
	//void renderPlane(btRigidBody* plane);
	//void renderBox(btRigidBody* box);
	btStaticPlaneShape *getPlaneRigidBody();
};
