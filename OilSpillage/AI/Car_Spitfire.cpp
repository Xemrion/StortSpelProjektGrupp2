#include "../States/PlayingGameState.h"
#include "Car_Spitfire.h"

void Spitfire::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Spitfire::inRange, std::ref(*this)));

	Behavior& idle = bt.getAction();
	idle.addAction(std::bind(&Spitfire::setIdleState, std::ref(*this)));

	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Spitfire::setChaseState, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(idle);

	sequence.addChildren(inRange);
	sequence.addChildren(chase);
}

Spitfire::Spitfire(float x, float z, Physics* physics)
	: DynamicActor(x,z)
{
	this->direction = Vector3(1, 0, 0);
	//this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	this->deltaTime = 0;
	setUpActor();
	throttleInputStrength = 0;
	car = new AIVehicle();
	car->init(physics);
	car->getVehicle()->setPosition(Vector3(position.x, 0 - 1.2f, position.z));
	car->getVehicleBody1()->setPosition(Vector3(position.x, 0 - 1.2f + 0.65f, position.z));
	this->defaultStats = VehicleStats::AIAttacker;
	this->updatedStats = this->defaultStats;
	setHealth(this->updatedStats.maxHealth);
	//Game::getGraphics().loadModel("Entities/Dummy_Player_Car1");
	//this->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Player_Car1");
	//this->setMaterial(Game::getGraphics().getMaterial("Entities/Dummy_Player_Car1"));
	//Game::getGraphics().addToDraw(&mesh);

}

Spitfire::Spitfire()
{
	/* should not go inside here*/
	this->deltaTime = 0;
	//this->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Player_Car1");
	//this->setMaterial(Game::getGraphics().getMaterial("Entities/Dummy_Player_Car1"));
}

Spitfire::~Spitfire()
{
	delete this->car;
}

Status Spitfire::inRange()
{
	Status status;

	if ((car->getVehicle()->getPosition() - targetPos).Length() <= chaseRange)
	{
		status = Status::SUCCESS;
	}
	else
	{
		status = Status::FAILURE;
	}
	return status;
}

Status Spitfire::vehicleUpdate()
{
	this->updateVehicle();
	return Status::SUCCESS;
}

void Spitfire::updateVehicle()
{
	prevAccelForce = Vector3(car->getVehicle()->getRigidBody()->getLinearVelocity());
	car->update(deltaTime, throttleInputStrength, false, false, Vector2(-direction.x, -direction.z));
	Vector3 accelForce = Vector3(car->getVehicle()->getRigidBody()->getLinearVelocity().getX(), car->getVehicle()->getRigidBody()->getLinearVelocity().getY(), car->getVehicle()->getRigidBody()->getLinearVelocity().getZ()) - Vector3(prevAccelForce.x, prevAccelForce.y, prevAccelForce.z);
	car->setAccelForce(accelForce, deltaTime);
	car->setWheelRotation();
}

void Spitfire::move()
{
	direction = car->getVehicle()->getPosition() - destination;
	direction.Normalize();
	if ((car->getVehicle()->getPosition() - destination).Length() > 5)
	{
		throttleInputStrength += 0.025 * deltaTime;
		if (throttleInputStrength > 1)
		{
			throttleInputStrength = 1;
		}
	}
	/*else if ((car->getVehicle()->getPosition() - destination).Length() < 5)
	{
		throttleInputStrength -= 0.5 * deltaTime;
		if (throttleInputStrength < 0)
		{
			throttleInputStrength = 0;
		}
	}*/

}

void Spitfire::update(float dt, Vector3 targetPos)
{
	DynamicActor::update(dt, targetPos);
	updateVehicle();
	this->setPosition(car->getVehicle()->getPosition());
}

void Spitfire::followPath()
{
	if (path != nullptr)
	{

		if (path->size() > 0)
		{
			destination = path->at(path->size() - 1);
			if ((destination - car->getVehicle()->getPosition()).Length() < 15)
			{
				path->pop_back();
			}
		}
		else
		{
			destination = targetPos;
		}
	}
}
