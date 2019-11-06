#include "../States/PlayingGameState.h"
#include "Car_Spitfire.h"

void Spitfire::setUpActor()
{
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Spitfire::inRange, std::ref(*this)));

	Behavior& updateVehicle = bt.getAction();
	updateVehicle.addAction(std::bind(&Spitfire::vehicleUpdate, std::ref(*this)));


	root->addChildren(sequence);
	sequence.addChildren(inRange);
	sequence.addChildren(updateVehicle);
}

Spitfire::Spitfire(float x, float z, AStar* aStar):
	Actor(x, z, aStar)
{
	this->direction = Vector3(1, 0, 0);
	//this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	this->deltaTime = 0;
	setUpActor();
	throttleInputStrength = 0;
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

void Spitfire::update(float dt, Vector3 targetPos)
{
	this->deltaTime = dt;
	this->targetPos = targetPos;
	this->root->func();
}

void Spitfire::Init(Physics* physics)
{
	car = new Vehicle();
	car->init(physics);
	car->getVehicle()->setPosition(Vector3(position.x, 0 - 1.2f, position.z));
	car->getVehicleBody1()->setPosition(Vector3(position.x, 0 - 1.2f + 0.65f, position.z));
}

Status Spitfire::inRange()
{
	Status status = Status::SUCCESS;
	direction = car->getVehicle()->getPosition() - targetPos;
	direction.Normalize();
	if ((car->getVehicle()->getPosition() - targetPos).Length() > 5)
	{
		status = Status::SUCCESS;
		throttleInputStrength += 0.5 * deltaTime;
		if (throttleInputStrength > 1)
		{
			throttleInputStrength = 1;
		}
	}
	else if ((car->getVehicle()->getPosition() - targetPos).Length() < 5)
	{
		status = Status::SUCCESS;
		throttleInputStrength -= 0.5 * deltaTime;
		if (throttleInputStrength < 0)
		{
			throttleInputStrength = 0;
		}
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
