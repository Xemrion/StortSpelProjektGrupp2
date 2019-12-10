#include "Physics.h"
#include "Vehicle.h"
#include "AI/Actor.h"
static int gSolverMode = SOLVER_SIMD |
SOLVER_USE_WARMSTARTING |
 SOLVER_RANDMIZE_ORDER |
 SOLVER_INTERLEAVE_CONTACT_AND_FRICTION_CONSTRAINTS |
// SOLVER_USE_2_FRICTION_DIRECTIONS |
0;
Physics::Physics() :broadphase(new btDbvtBroadphase())
{
#ifndef _DEBUG
	collisionConfig = new btDefaultCollisionConfiguration();
	solver = new btSequentialImpulseConstraintSolver();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);

#else
	btSetTaskScheduler(btGetOpenMPTaskScheduler());
	btDefaultCollisionConstructionInfo cci;
	collisionConfig = new btDefaultCollisionConfiguration(cci);
	dispatcherMt = new btCollisionDispatcherMt(collisionConfig, 40);
	solverPool = new btConstraintSolverPoolMt(BT_MAX_THREAD_COUNT);
	solver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorldMt(dispatcherMt, broadphase, solverPool, solver, collisionConfig);
	world->getSolverInfo().m_solverMode = gSolverMode;
	world->getSolverInfo().m_numIterations = 10;

#endif // !DEBUG
	world->setForceUpdateAllAabbs(false);
	world->setGravity(btVector3(0, -10, 0));

	//temp plane inf
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(0, 0, 0));


	////set shape for object
	plane = new btStaticPlaneShape(btVector3(0, 1, 0), -1.5f);

	//set motionshape aka set postion
	btMotionState* motion = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	////body definition check doc
	btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);

	btRigidBody* body = new btRigidBody(info);
	body->setFriction(1);

	world->addRigidBody(body);

	bodies.push_back(body);

	/*gContactAddedCallback = callbackFunc;*/
}


Physics::~Physics()
{
	//osäker på om man deletar på detta sätt
	for (int i = 0; i < bodies.size(); i++)
	{
		btMotionState* motionState = bodies[i]->getMotionState();
		btCollisionShape* shape = bodies[i]->getCollisionShape();
		delete shape;
		delete motionState;
		this->world->removeRigidBody(bodies[i]);

		delete bodies[i];
	}
	for (int i = 0; i < pointJoints.size(); i++)
	{
		delete pointJoints[i];
	}
	delete solverPool;
	delete dispatcher;
	delete dispatcherMt;
	delete collisionConfig;
	delete solver;
	delete broadphase;
	delete world;

	bodies.clear();

}

void Physics::update(float deltaTime)
{	
#ifdef _DEBUG
	OutputDebugString(("Rigidbodys in the world: " + std::to_string(this->bodies.size()) + "\n").c_str());
	OutputDebugString(("Joints in the world: " + std::to_string(this->pointJoints.size()) + "\n").c_str());
#endif // _DEBUG

	this->world->stepSimulation(deltaTime, 6, 1. / 120.);
}

btRigidBody* Physics::addSphere(float radius, btVector3 Origin, float mass, GameObject* obj)
{	//add object set transform
	btTransform t; //
	t.setIdentity();
	t.setOrigin(btVector3(Origin));
	btSphereShape* sphere = new btSphereShape(radius); //raduius

	btVector3 inertia(0, 0, 0);
	if (mass != 0.0f) {
		sphere->calculateLocalInertia(mass, inertia);
	}

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);

	btRigidBody* body = new btRigidBody(info);

	bodies.push_back(body);
	body->setUserPointer(obj);

	this->world->addRigidBody(body);



	return body;
}

btRigidBody* Physics::addBox(btVector3 Origin, btVector3 size, float mass, GameObject* objects)
{
	//add object set transform
	btTransform t; //
	t.setIdentity();
	t.setOrigin(btVector3(Origin));
	btBoxShape* box = new btBoxShape(size);


	btVector3 inertia(0, 0, 0);
	if (mass != 0.0f) {
		box->calculateLocalInertia(mass, inertia);
	}
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, box, inertia);
	btRigidBody* body = new btRigidBody(info);
	body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	bodies.push_back(body);
	body->setUserPointer(objects);

	this->world->addRigidBody(body);


	return body;
}

btRigidBody* Physics::addCylinder(btVector3 Origin, btVector3 size, float mass)
{
	btTransform t; //
	t.setIdentity();
	t.setOrigin(btVector3(Origin));
	btCylinderShape* cylinder = new btCylinderShape(size); //raduius

	btVector3 inertia(0, 0, 0);
	if (mass != 0.0f) {
		cylinder->calculateLocalInertia(mass, inertia);
	}

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, cylinder, inertia);

	btRigidBody* body = new btRigidBody(info);

	bodies.push_back(body);

	this->world->addRigidBody(body);

	return body;
}

btRigidBody* Physics::addCapsule(btScalar radius, btVector3 Origin, btScalar height, float mass)
{
	btTransform t; //
	t.setIdentity();
	t.setOrigin(btVector3(Origin));
	btCapsuleShapeZ* capsule = new btCapsuleShapeZ(radius,height); //raduius

	btVector3 inertia(0, 0, 0);
	if (mass != 0.0f) {
		capsule->calculateLocalInertia(mass, inertia);
	}

	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, capsule, inertia);

	btRigidBody* body = new btRigidBody(info);

	bodies.push_back(body);

	this->world->addRigidBody(body);

	return body;
}

btGeneric6DofSpring2Constraint* Physics::addSpring(btRigidBody* box1, btRigidBody* box2)
{
	btGeneric6DofSpring2Constraint* spring = new btGeneric6DofSpring2Constraint(
		*box1, *box2,
		btTransform(btQuaternion::getIdentity(), { 0.0f, -1.5f, 0.0f }),
		btTransform(btQuaternion::getIdentity(), { 0.0f,  0.0f, 0.0f }));
	springs.push_back(spring);

	this->world->addConstraint(spring);

	return spring;
}

btPoint2PointConstraint* Physics::addPointJoint(btRigidBody* box1, btRigidBody* box2)
{
	btPoint2PointConstraint* pointJoint = new btPoint2PointConstraint(*box1, *box2, btVector3(0, 0.00f, 0), btVector3(0, -0.55f, 0));
	pointJoints.push_back(pointJoint);

	this->world->addConstraint(pointJoint);

	return pointJoint;
}

btRaycastVehicle* Physics::addVehicle(btRaycastVehicle* vehicle)
{
	vehicles.push_back(vehicle);
	this->world->addVehicle(vehicle);

	return vehicle;
}

bool Physics::deleteRigidBody(btRigidBody* rb)
{

	for (int i = 0; i < this->bodies.size(); i++)
	{
		if (bodies[i] == rb)
		{
			this->world->removeRigidBody(rb);
			bodies.erase(bodies.begin() + i);
			delete rb;
			return true;
		}
	}
	return false;
}
bool Physics::deletePointJoint(btPoint2PointConstraint* pointJoint)
{
	for (int i = 0; i < this->pointJoints.size(); i++)
	{
		if (pointJoints[i] == pointJoint)
		{
			this->world->removeConstraint(pointJoint);
			pointJoints.erase(pointJoints.begin() + i);
			delete pointJoint;

			return true;
		}
	}
	return false;
}

bool Physics::callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1,
	int index1, const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	return false;
}

btStaticPlaneShape* Physics::getPlaneRigidBody()
{
	return plane;
}
