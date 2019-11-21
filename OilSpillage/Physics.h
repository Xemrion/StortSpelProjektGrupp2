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
#include "BulletDynamics/MLCPSolvers/btLemkeSolver.h"
#include "LinearMath/btAlignedObjectArray.h"
#include "LinearMath/btPoolAllocator.h"
#include "btBulletCollisionCommon.h"
#include "BulletCollision/CollisionDispatch/btCollisionDispatcherMt.h"
#include "BulletDynamics/Dynamics/btSimulationIslandManagerMt.h"  // for setSplitIslands()
#include "BulletDynamics/Dynamics/btDiscreteDynamicsWorldMt.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolverMt.h"
#include "BulletDynamics/ConstraintSolver/btSequentialImpulseConstraintSolver.h"
#include "BulletDynamics/ConstraintSolver/btNNCGConstraintSolver.h"
#include "BulletDynamics/MLCPSolvers/btMLCPSolver.h"
#include "BulletDynamics/MLCPSolvers/btSolveProjectedGaussSeidel.h"
#include "BulletDynamics/MLCPSolvers/btDantzigSolver.h"
#include "BulletDynamics/MLCPSolvers/btLemkeSolver.h"
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
	btClock realClock;
	btConstraintSolver* m_solver;
	SolverType m_solverType;
	btDiscreteDynamicsWorldMt* worldMt;
	btCollisionDispatcherMt* dispatcherMt;
	btDiscreteDynamicsWorld* world;
	btCollisionDispatcher* dispatcher;
	btBroadphaseInterface* broadphase;
	btConstraintSolver* solver;
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
	void moveBody(btRigidBody* rigidbody,float x, float y, float z);
	void update(float deltaTime);
	btRigidBody* addSphere(float radius, btVector3 Origin, float mass, void* obj = nullptr);
	btRigidBody* addBox(btVector3 Origin, btVector3 size, float mass, void* obj = nullptr);
	btRigidBody* addCylinder(btVector3 Origin, btVector3 size, float mass);
	btGeneric6DofSpring2Constraint* addSpring(btRigidBody* box1, btRigidBody* box2);
	btPoint2PointConstraint* addPointJoint(btRigidBody* box1, btRigidBody* box2);
	btRaycastVehicle* addVehicle(btRaycastVehicle* vehicle);
	/*btRigidBody* addPlayer(btVector3 Origin, btVector3 size, float mass,Player *player);*/

	bool DeleteRigidBody(btRigidBody* rb);
	bool deletePointJoint(btPoint2PointConstraint* pointJoint);
	static bool callbackFunc(btManifoldPoint& cp, const btCollisionObjectWrapper* obj1, int id1, int index1, const btCollisionObjectWrapper* obj2,
		int id2, int index2);
	//void renderSphere(btRigidBody* sphere);
	//void renderPlane(btRigidBody* plane);
	//void renderBox(btRigidBody* box);
	btStaticPlaneShape* getPlaneRigidBody();
};

#endif // !PHYSICS_H
