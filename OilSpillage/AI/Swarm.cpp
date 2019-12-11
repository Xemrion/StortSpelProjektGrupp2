#include "../States/PlayingGameState.h"
#include "Swarm.h"

Swarm::Swarm()
{

}

Swarm::Swarm(float x, float z, Physics* physics)
	: DynamicActor(x, z, physics), Melee(&position, &velocity, &deltaTime)
{
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	Game::getGraphics().addToDraw(this);

	this->stats = VehicleStats::AISwarm;

	float newHealth = stats.maxHealth;
	scaling(newHealth, 1.3);
	setMaxHealth(newHealth);
	setHealth(newHealth);

	scaling(stats.speed, 1.1);

	this->mesh = Game::getGraphics().getMeshPointer("Entities/Drone");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Drone"));
	this->weapon = WeaponHandler::getWeapon(WeaponType::aiMelee);
	createRigidbody(physics);
	this->setPoints(50 * (1 + (0.1 * Game::getGameInfo().nrOfClearedStages)));
	scaling(weapon.damage, 1.1f);
}

void Swarm::update(float dt, const Vector3& targetPos)
{
	DynamicActor::update(dt, targetPos);
	if ((position - targetPos).Length() < 2)
	{
		meleeAttack();
	}
}

void Swarm::createRigidbody(Physics* physics)
{
	btRigidBody* tempo = physics->addSphere(0.7f, btVector3(position.x, position.y, position.z), 0.5f, this);
	setRigidBody(tempo);
	getRigidBody()->activate();
	getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	getRigidBody()->setFriction(0);
	getRigidBody()->setLinearFactor(btVector3(1, 0, 1));
}

Swarm::~Swarm()
{
	Game::getGraphics().removeFromDraw(this);
	if (this->isDead())
	{
		Game::getGameInfo().nrOfSwarm++;
	}
}
Vector3 Swarm::calculateVelocity()
{
	Vector3 desiredDirection = destination - position;
	Vector3 offsetVec;
	Vector3 eliminatingVec = Vector3(0.0f, -2.0f, 0.0f);
	//standard group movement
	if (desiredDirection.Length() <= 10 && desiredDirection.Length() > 2.5f)
	{
		this->stats.speed = 11.0;
		Vector3 crossVector = Vector3(position.x - destination.x, 0.0f, position.z - destination.z);
		offsetVec = crossVector.Cross(eliminatingVec);
		offsetVec.Normalize();
		offsetVec *= 10;
		if (duty == 0)
		{
			desiredDirection = (destination - offsetVec) - position;
		}
		else if (duty == 1)
		{
			desiredDirection = (destination + offsetVec) - position;
		}
	}
	else
	{
		this->stats.speed = 7;
	}

	return  desiredDirection;
}

