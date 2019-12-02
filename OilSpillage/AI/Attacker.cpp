#include "Attacker.h"
#include"../States/PlayingGameState.h"
Attacker::Attacker()
{
}

Attacker::Attacker(float x, float z, int weaponType, Physics* physics)
	:DynamicActor(x, z,physics), Ranged(&this->position, &this->targetPos, &this->velocity,&this->deltaTime, weaponType)
{
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	setUpActor();
	Game::getGraphics().addToDraw(this);

	this->stats = VehicleStats::AIAttacker;
	//setHealth(this->stats.maxHealth * (1 + (0.3 * Game::getGameInfo().nrOfClearedStages)));
	float newHealth = stats.maxHealth;
	scaling(newHealth, 1.3);
	setMaxHealth(newHealth);
	setHealth(newHealth);
	Game::getGraphics().loadModel("Entities/Roller_Melee");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Roller_Melee");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Roller_Melee"));
	this->attackRange = 10;
	createRigidbody(physics);
	this->setPoints(100 * (1 + (0.2 * Game::getGameInfo().nrOfClearedStages)));
	//this->weapon.damage = this->weapon.damage * (1 + (0.1 * Game::getGameInfo().nrOfClearedStages));
	scaling(weapon.damage, 1.1);
}

void Attacker::update(float dt, const Vector3& targetPos)
{
	DynamicActor::update(dt, targetPos);
	this->updateBullets(dt);
}
void Attacker::createRigidbody(Physics* physics)
{
	btRigidBody* tempo = physics->addSphere(1.0f, btVector3(position.x, position.y, position.z), 0.5f, this);
	setRigidBody(tempo, physics);
	getRigidBody()->activate();
	getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	getRigidBody()->setFriction(0);
	getRigidBody()->setLinearFactor(btVector3(1, 0, 1));
}
Attacker::~Attacker()
{
	Game::getGraphics().removeFromDraw(this);
	if(this->isDead())
	{
		Game::getGameInfo().nrOfAttackers++;
	}
}

void Attacker::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();
	Selector& selector = bt.getSelector();
	Sequence& seq2 = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Attacker::inAttackRange, std::ref(*this)));
	//Behavior& waitTimer = bt.getAction();
	//waitTimer.addAction(std::bind(&Attacker::WaitTime, std::ref(*this)));
	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Attacker::setChaseState, std::ref(*this)));
	Behavior& roam = bt.getAction();
	roam.addAction(std::bind(&Attacker::setIdleState, std::ref(*this)));
	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Attacker::shoot, std::ref(*this)));
	Behavior& inAggroRange = bt.getAction();
	inAggroRange.addAction(std::bind(&Attacker::inAggroRange, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(roam);

	sequence.addChildren(inAggroRange);
	sequence.addChildren(selector);

	selector.addChildren(seq2);
	selector.addChildren(chase);

	seq2.addChildren(inRange);
	
	//seq2.addChildren(waitTimer);
	seq2.addChildren(shoot);
}

Vector3 Attacker::seek()
{
	Vector3 desiredDirection;
	Vector3 offsetVec;
	//standard group movement
	if (!vActive)
	{
		desiredDirection -= position - destination;
		//desired *= maxSpeed;
		if (this->stats.maxSpeed != 3.0)
		{
			this->stats.maxSpeed = 3.0;
		}
	}

	//strafe to the left of player
	else if (vActive)
	{
		Vector3 crossVector = Vector3(position.x - destination.x, 0.0f, position.z - destination.z);
		crossVector.Normalize();
		crossVector *= -10;
		desiredDirection -= position - (destination - crossVector);
	}
	

	acceleration = desiredDirection - velocity;
	if (acceleration.Length() > maxForce)
	{
		acceleration /= acceleration.Length();
	}
	//vActive = false;
	return acceleration;
}

