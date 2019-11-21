#include "Physics.h"
#include "Vehicle.h"
#include "AI/Actor.h"
static SolverType gSolverType = SOLVER_TYPE_SEQUENTIAL_IMPULSE_MT;
static int gSolverMode = SOLVER_SIMD |
SOLVER_USE_WARMSTARTING |
// SOLVER_RANDMIZE_ORDER |
// SOLVER_INTERLEAVE_CONTACT_AND_FRICTION_CONSTRAINTS |
// SOLVER_USE_2_FRICTION_DIRECTIONS |
0;
btConstraintSolver* createSolverByType(SolverType t)
{
	btMLCPSolverInterface* mlcpSolver = NULL;
	switch (t)
	{
	case SOLVER_TYPE_SEQUENTIAL_IMPULSE:
		return new btSequentialImpulseConstraintSolver();
	case SOLVER_TYPE_SEQUENTIAL_IMPULSE_MT:
		return new btSequentialImpulseConstraintSolverMt();
	case SOLVER_TYPE_NNCG:
		return new btNNCGConstraintSolver();
	case SOLVER_TYPE_MLCP_PGS:
		mlcpSolver = new btSolveProjectedGaussSeidel();
		break;
	case SOLVER_TYPE_MLCP_DANTZIG:
		mlcpSolver = new btDantzigSolver();
		break;
	case SOLVER_TYPE_MLCP_LEMKE:
		mlcpSolver = new btLemkeSolver();
		break;
	default:
	{
	}
	}
	if (mlcpSolver)
	{
		return new btMLCPSolver(mlcpSolver);
	}
	return NULL;
}
Physics::Physics() :broadphase(new btDbvtBroadphase())
{
#ifdef _DEBUG
	collisionConfig = new btDefaultCollisionConfiguration();
	solver = new btSequentialImpulseConstraintSolver();
	dispatcher = new btCollisionDispatcher(collisionConfig);
	world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
	world->setForceUpdateAllAabbs(false);
	world->setGravity(btVector3(0, -10, 0));
#else
	btDefaultCollisionConstructionInfo cci;
	cci.m_defaultMaxPersistentManifoldPoolSize = 80000;
	cci.m_defaultMaxCollisionAlgorithmPoolSize = 80000;
	collisionConfig = new btDefaultCollisionConfiguration(cci);
	dispatcherMt = new btCollisionDispatcherMt(collisionConfig, 40);

	btConstraintSolverPoolMt* solverPool;
	{
		SolverType poolSolverType = m_solverType;
		if (poolSolverType == SOLVER_TYPE_SEQUENTIAL_IMPULSE_MT)
		{
			// pool solvers shouldn't be parallel solvers, we don't allow that kind of
			// nested parallelism because of performance issues
			poolSolverType = SOLVER_TYPE_SEQUENTIAL_IMPULSE;
		}
		btConstraintSolver* solvers[BT_MAX_THREAD_COUNT];
		int maxThreadCount = BT_MAX_THREAD_COUNT;
		for (int i = 0; i < maxThreadCount; ++i)
		{
			solvers[i] = createSolverByType(poolSolverType);
		}
		solverPool = new btConstraintSolverPoolMt(solvers, maxThreadCount);
		m_solver = solverPool;
	}
	btSequentialImpulseConstraintSolverMt* solverMt = new btSequentialImpulseConstraintSolverMt();
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();

	if (m_solverType == SOLVER_TYPE_SEQUENTIAL_IMPULSE_MT)
	{
		solverMt = new btSequentialImpulseConstraintSolverMt();
	}

	world = new btDiscreteDynamicsWorldMt(dispatcherMt, broadphase, solverPool, solver, collisionConfig);
	//worldMt->getSolverInfo().m_solverMode = gSolverMode;
	world->setForceUpdateAllAabbs(false);
	world->setGravity(btVector3(0, -10, 0));
	world->getSolverInfo().m_solverMode = gSolverMode;
	world->getSolverInfo().m_numIterations = 10;
#endif // !DEBUG

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

	gContactAddedCallback = callbackFunc;
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
	delete dispatcher;
	delete collisionConfig;
	delete solver;
	delete broadphase;
	delete world;

	bodies.clear();

}

void Physics::moveBody(btRigidBody* rigidbody, float x, float y, float z)
{
	btTransform transform = rigidbody->getCenterOfMassTransform();
	transform.setOrigin(btVector3(x, y, z));
	rigidbody->setCenterOfMassTransform(transform);
}

void Physics::update(float deltaTime)
{

	//this->world->stepSimulation(deltaTime);

	this->world->stepSimulation(deltaTime, 2, 1. / 120.);

	//this->world->stepSimulation(deltaTime, 0);
	//this->world->stepSimulation(btScalar(deltaTime));
}

btRigidBody* Physics::addSphere(float radius, btVector3 Origin, float mass, void* obj)
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

btRigidBody* Physics::addBox(btVector3 Origin, btVector3 size, float mass, void* objects)
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
	//body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
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
	//pointJoint->enableFeedback(true);
	/*btJointFeedback* hej = new btJointFeedback;
	hej->m_appliedForceBodyA = btVector3(1, 0, 1);
	pointJoint->setJointFeedback(hej);*/
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

bool Physics::DeleteRigidBody(btRigidBody* rb)
{

	for (int i = 0; i < this->bodies.size(); i++)
	{
		if (bodies[i] == rb)
		{
			this->world->removeRigidBody(bodies[i]);

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

			return true;
		}
	}
	return false;
}

bool Physics::callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1,
	int index1, const btCollisionObjectWrapper* obj2, int id2, int index2)
{
	Vehicle* playerPtr = static_cast<Vehicle*>(obj1->getCollisionObject()->getUserPointer());
	Actor* enemyPtr = static_cast<Actor*>(obj2->getCollisionObject()->getUserPointer());

	//if (playerPtr == nullptr || enemyPtr == nullptr ? true : !playerPtr->isPlayer())
	//{
	//	playerPtr = static_cast<Vehicle*>(obj2->getCollisionObject()->getUserPointer());
	//	enemyPtr = static_cast<Actor*>(obj1->getCollisionObject()->getUserPointer());

	//	if (playerPtr == nullptr || enemyPtr == nullptr ? true : !playerPtr->isPlayer())
	//	{
	//		return false;
	//	}
	//}

	//if (enemyPtr != nullptr)
	//{
	//	if (playerPtr->getPowerUpTimer(PowerUpType::Star) > 0.0)
	//	{
	//		enemyPtr->setHealth(0);
	//	}

	//	return true;
	//}
	return false;
}

btStaticPlaneShape* Physics::getPlaneRigidBody()
{
	return plane;
}
