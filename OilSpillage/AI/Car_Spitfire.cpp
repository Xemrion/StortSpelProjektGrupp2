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
	Actor(x, z, nullptr)
{
	this->forward = Vector3(1, 0, 0);
	this->setScale(Vector3(0.01f, 0.01f, 0.01f));
	this->deltaTime = 0;
	setUpActor();
	//Game::getGraphics().loadModel("Entities/Dummy_Player_Car1");
	//this->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Player_Car1");
	//this->setMaterial(Game::getGraphics().getMaterial("Entities/Dummy_Player_Car1"));
	//Game::getGraphics().addToDraw(&mesh);
	
	car = new Vehicle();
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

Status Spitfire::inRange()
{
	Status status;
	if ((getPosition() - targetPos).Length() > 15)
	{
		status = Status::FAILURE;
	}
	else
	{
		status = Status::SUCCESS;
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
	/*Vehicle->update()*/
}
