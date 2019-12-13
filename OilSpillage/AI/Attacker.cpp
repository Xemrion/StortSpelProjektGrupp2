#include "Attacker.h"
#include"../States/PlayingGameState.h"
Attacker::Attacker()
{
}

Attacker::Attacker(float x, float z, int weaponType, Physics* physics)
	:DynamicActor(x, z, physics), Ranged(this->createRigidbody(physics), &this->targetPos, &this->velocity, &this->deltaTime, weaponType)
{
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	setUpActor();
	Game::getGraphics().addToDraw(this);

	this->stats = VehicleStats::AIAttacker;
	float newHealth = stats.maxHealth;
	scaling(newHealth, 1.3);
	setMaxHealth(newHealth);
	setHealth(newHealth);

	this->mesh = Game::getGraphics().getMeshPointer("Entities/Roller_Melee");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Roller_Melee"));
	this->attackRange = 12;
	this->setPoints(100 * (1 + (0.2 * Game::getGameInfo().nrOfClearedStages)));
	scaling(weapon.damage, 1.1);
}

void Attacker::update(float dt, const Vector3& targetPos)
{
	
	DynamicActor::update(dt, targetPos);
	this->updateBullets(dt);
}
btRigidBody* Attacker::createRigidbody(Physics* physics)
{
	btRigidBody* tempo = physics->addSphere(1.0f, btVector3(), 0.5f, this);
	setRigidBody(tempo);
	getRigidBody()->activate();
	getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	getRigidBody()->setFriction(0);
	getRigidBody()->setLinearFactor(btVector3(1, 0, 1));

	return getRigidBody();
}
Attacker::~Attacker()
{
	Game::getGraphics().removeFromDraw(this);
	if (this->isDead())
	{
		Game::getGameInfo().nrOfAttackers++;
	}
}

void Attacker::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Attacker::inAttackRange, std::ref(*this)));
	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Attacker::shoot, std::ref(*this)));

	root->addChildren(sequence);
	sequence.addChildren(inRange);
	sequence.addChildren(shoot);

}

Vector3 Attacker::calculateVelocity()
{
	//Move away from player if within 11, move towards it if further than 12, stand still if between the two
	Vector3 position = this->getPosition();
	Vector3 desiredDirection = destination - position;
	if (destination == targetPos)
	{
		float deltaZ = destination.z - position.z;
		float deltaX = destination.x - position.x;
		float distance = (deltaX * deltaX) + (deltaZ * deltaZ);
		if (distance <= 11 * 11)
		{
			desiredDirection = -desiredDirection;
		}
		else if (distance < 12 * 12)
		{
			desiredDirection = Vector3();
		}

	}
	return desiredDirection;
}

