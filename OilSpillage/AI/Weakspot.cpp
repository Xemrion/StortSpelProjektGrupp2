#include "Weakspot.h"

Weakspot::Weakspot()
{

}

Weakspot::Weakspot(float x, float z, int weaponType, Physics* physics)
	:DynamicActor(x, z, physics)
{
	this->setScale(Vector3(1.0f, 1.0f, 1.0f));
	this->setUpActor();
	Game::getGraphics().addToDraw(this);

	this->stats = VehicleStats::AIPart;
	setHealth(this->stats.maxHealth);
	Game::getGraphics().loadModel("Entities/Barrel");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Barrel");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Barrel"));
}

Weakspot::~Weakspot()
{
	Game::getGraphics().removeFromDraw(this);
}

void Weakspot::update(float dt, Vector3 targetPos)
{
	//Calls this update and not place, where, how?
	//this->position = targetPos;
}

void Weakspot::setUpActor()
{
	//Selectors
	this->root = &bt.getSelector();
	Selector& selector2 = bt.getSelector();
	//Sequences
	Sequence& sequence1 = bt.getSequence();

	//Movement
	Behavior& roam = bt.getAction();
	roam.addAction(std::bind(&Weakspot::setIdleState, std::ref(*this)));

	//Behaviour Tree
	root->addChildren(roam);
	root->addChildren(selector2);
}

void Weakspot::place(Vector3 thisPosition)
{
	this->setPosition(thisPosition);
	//this->position = thisPosition;
}