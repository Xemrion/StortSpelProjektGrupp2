#include "Physics.h"
#include "PG/utils.hpp"

using namespace DirectX::SimpleMath;

Physics::Physics() :
	broadphase      { std::make_unique<btDbvtBroadphase>()                    },
	collisionConfig { std::make_unique<btDefaultCollisionConfiguration>()     },
	solver          { std::make_unique<btSequentialImpulseConstraintSolver>() }
{
   dispatcher = std::make_unique<btCollisionDispatcher>( collisionConfig.get() );

	world      = new btDiscreteDynamicsWorld ( dispatcher.get(),
	                                           broadphase.get(),
	                                           solver.get(),
	                                           collisionConfig.get() );
	world->setGravity( btVector3(0, -10, 0) );
	
	plane = std::make_unique<btStaticPlaneShape>( btVector3{0, 1, 0}, 3 );

	// set motionstate aka set position
	auto transform = btTransform { btQuaternion(0, 0, 0, 1), btVector3(0, -1, 0) };
	motions.push_back( std::make_unique<btDefaultMotionState>(transform) );
	auto motion = motions.back().get();
	
	// body definition check doc
	auto info = btRigidBody::btRigidBodyConstructionInfo { .0f, motion, plane.get() };
	bodies.push_back( std::make_unique<btRigidBody>(info) );
	auto body = bodies.back().get();
	body->setFriction( 1 );
	world->addRigidBody(body);

//gContactAddedCallback = callbackFunc;
}


Physics::~Physics() {
	delete world;
}

void Physics::update( float deltaTime )
{
	//world->stepSimulation( deltaTime );
	world->stepSimulation( deltaTime, 10000, 1.0f / 240.0f );
	//world->stepSimulation( deltaTime, 0 );
	//world->stepSimulation( btScalar(deltaTime) );
}

btRigidBody *Physics::addSphere( float radius, btVector3 origin, float mass )
{
	spheres.push_back( std::make_unique<btSphereShape>(radius) );
	auto sphere = spheres.back().get();
	
	btVector3 inertia(0, 0, 0);
	if ( mass != .0f )
		sphere->calculateLocalInertia( mass, inertia );
	
	// add object set transform
	btTransform transform {};
	transform.setIdentity();
	transform.setOrigin( btVector3(origin) );
	
	motions.push_back( std::make_unique<btDefaultMotionState>(transform) );
	auto motion = motions.back().get();
	
	auto info = btRigidBody::btRigidBodyConstructionInfo( mass, motion, sphere, inertia );
	
	bodies.push_back( std::make_unique<btRigidBody>(info) );
	auto body = bodies.back().get();
	
	world->addRigidBody( body );
	
	return body;
}

//btRigidBody* Physics::addBox(btVector3 origin, btVector3 size, float mass)
//{
//	return nullptr;
//}

btRigidBody *Physics::addBox( btVector3 origin, btVector3 size, float mass )
{
	boxes.push_back( std::make_unique<btBoxShape>(size) );
	auto box = boxes.back().get();
	
	btVector3 inertia { 0, 0, 0 };
	if ( mass != .0f )
		box->calculateLocalInertia( mass, inertia );
	
	// add object set transform
	btTransform transform {};
	transform.setIdentity();
	transform.setOrigin( btVector3(origin)) ;

	motions.push_back( std::make_unique<btDefaultMotionState>(transform) );
	auto motion = motions.back().get();
	
	auto info = btRigidBody::btRigidBodyConstructionInfo( mass, motion, box, inertia );
	
	bodies.push_back( std::make_unique<btRigidBody>(info) );
	auto body = bodies.back().get();
	body->setCollisionFlags( body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK );
	// body->setUserPointer(objects);
	
	world->addRigidBody( body );
	
	return body;
}

//btRigidBody* Physics::addPlayer(btVector3 origin, btVector3 size, float mass, Player *player)
//{
//	//add object set transform
//	btTransform t; //
//	t.setIdentity();
//	t.setOrigin(btVector3(origin));
//	btBoxShape* box = new btBoxShape(size);
//
//	btVector3 inertia(0, 0, 0);
//	if (mass != 0.0f) {
//		box->calculateLocalInertia(mass, inertia);
//	}
//	btMotionState* motion = new btDefaultMotionState(t);
//	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, box, inertia);
//	btRigidBody* body = new btRigidBody(info);
//	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
//	this->world->addRigidBody(body);
//	
//	bodies.push_back(body);
//	body->setUserPointer(player);
//
//	return body;
//}

void Physics::deleteRigidBody( btRigidBody *rb )
{
	//   util::erase_if( bodies, [rb](auto const &p){ return p.get() == rb; } );
	for ( auto &body : bodies ) {
		if ( auto e = body.get();  e == rb ) {
			util::erase_if( motions, [e]( auto &p ) {
			                            return p.get() == e->getMotionState();
			                         } );
			util::erase_if( shapes, [e]( auto &p ) {
			                           return p.get() == e->getCollisionShape();
			                        } );
			util::erase( bodies, body );
			world->removeRigidBody( e ); // TODO: rearrange?
			//return true;
		}
	}
	//return false;
}

//bool Physics::callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2, int id2, int index2)
//{
//	
//	
//	
//	
//	//if (obj1->getCollisionObject()->getUserPointer() == ((Player*)obj1->getCollisionObject()->getUserPointer()))
//	//{
//	//	if (((Player*)obj1->getCollisionObject()->getUserPointer()) != nullptr) {
//	//		bool ishit = ((Player*)obj1->getCollisionObject()->getUserPointer())->getHit();
//	//		
//	//	}
//	//	if (((Objects*)obj2->getCollisionObject()->getUserPointer()) != nullptr) {
//	//		OutputDebugStringA("1\n");
//	//		if (((Player*)obj2->getCollisionObject()->getUserPointer()) != nullptr) {
//	//			((Player*)obj2->getCollisionObject()->getUserPointer())->setGrounded(true);
//	//		}
//	//	}
//	//	/*else {
//	//		((Player*)obj1->getCollisionObject()->getUserPointer())->setGrounded(false);
//	//	}*/
//	//}
//	//return false;
//}

btStaticPlaneShape *Physics::getPlaneRigidBody()
{
	return plane.get();
}

//void Physics::renderSphere(btRigidBody* sphere)
//{
//	if (sphere->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXYTYPE) {
//		return;
//	}
//	float r = ((btSphereShape*)sphere->getCollisionShape())->getRadius();
//	btTransform t;
//	sphere->getMotionState()->getWorldTransform(t);
//}
//
//void Physics::renderPlane(btRigidBody* plane)
//{
//	if (plane->getCollisionShape()->getShapeType() != STATIC_PLANE_PROXYTYPE) {
//		return;
//	}
//	btTransform t;
//	plane->getMotionState()->getWorldTransform(t);
//}
//
//void Physics::renderBox(btRigidBody* box)
//{
//	if (box->getCollisionShape()->getShapeType() != SPHERE_SHAPE_PROXYTYPE) {
//		return;
//	}
//	btVector3 extent = ((btBoxShape*)box->getCollisionShape())->getHalfExtentsWithoutMargin();
//	btTransform t;
//	box->getMotionState()->getWorldTransform(t);
//}
