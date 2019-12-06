#ifndef PHYSICS_H
#define PHYSICS_H

#include "BulletSDK/include/btBulletDynamicsCommon.h"
#include <vector>
#include <iostream>
#include <string>
#include<DirectXMath.h>
#include<d3d11.h>
#include<SimpleMath.h>
#include <algorithm>
#include "BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.h"
#include "BulletDynamics/MLCPSolvers/btMLCPSolver.h"
#include "BulletDynamics/MLCPSolvers/btSolveProjectedGaussSeidel.h"
#include "BulletDynamics/MLCPSolvers/btDantzigSolver.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "LinearMath/btPoolAllocator.h"
#include "LinearMath/TaskScheduler/btThreadSupportInterface.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h"
#include "BulletDynamics/Dynamics/btSimulationIslandManagerMt.h"  // for setSplitIslands()
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h"
#include "BulletDynamics/MLCPSolvers/btLemkeSolver.h"
#define BT_THREADSAFE 1
#define BT_USE_PPL 1
using namespace DirectX::SimpleMath;
enum SolverType
{
	SOLVER_TYPE_SEQUENTIAL_IMPULSE,
	SOLVER_TYPE_SEQUENTIAL_IMPULSE_MT,
	SOLVER_TYPE_NNCG,
	SOLVER_TYPE_MLCP_PGS,
	SOLVER_TYPE_MLCP_DANTZIG,
	SOLVER_TYPE_MLCP_LEMKE,

	SOLVER_TYPE_COUNT
};
class Physics
{
private:
	btConstraintSolverPoolMt* solverPool;
	btSequentialImpulseConstraintSolver* solver;
	btConstraintSolver* m_solver;
	SolverType m_solverType;
	btCollisionDispatcherMt* dispatcherMt;
	btDiscreteDynamicsWorld* world;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* broadphase;
	btCollisionConfiguration* collisionConfig;
	std::vector<btRigidBody*> bodies;
	std::vector<btGeneric6DofSpring2Constraint*> springs;
	std::vector<btPoint2PointConstraint*> pointJoints;
	std::vector<btRaycastVehicle*> vehicles;

	std::vector<btCollisionShape*> shapes;

	btStaticPlaneShape* plane;
public:
	Physics();
	~Physics();
	void teleportRigidbody(Vector3 newPos, btRigidBody* body);
	void update(float deltaTime);
	btRigidBody* addSphere(float radius, btVector3 Origin, float mass, GameObject* obj = nullptr);
	btRigidBody* addBox(btVector3 Origin, btVector3 size, float mass, GameObject* obj = nullptr);
	btRigidBody* addCylinder(btVector3 Origin, btVector3 size, float mass);
	btRigidBody* addCapsule(btScalar radius, btVector3 Origin, btScalar height, float mass);
	btGeneric6DofSpring2Constraint* addSpring(btRigidBody* box1, btRigidBody* box2);
	btPoint2PointConstraint* addPointJoint(btRigidBody* box1, btRigidBody* box2);
	btRaycastVehicle* addVehicle(btRaycastVehicle* vehicle);

	bool DeleteRigidBody(btRigidBody* rb);
	bool deletePointJoint(btPoint2PointConstraint* pointJoint);
	static bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2,
		int id2, int index2);

	btStaticPlaneShape* getPlaneRigidBody();
};

#endif 
