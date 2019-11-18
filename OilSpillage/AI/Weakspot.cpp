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

void Weakspot::update(float dt, btVector3 velocityVec)
{
	//Calls this update and not place, where, how?
	//this->move(velocityVec);
}

void Weakspot::startPos(Vector3 startPos)
{
	this->setPosition(startPos);
}

void Weakspot::shortMove(Vector3 posVec)
{
	Vector3 newPos;
	newPos.x = this->getPosition().x + posVec.x;
	newPos.y = this->getPosition().y + posVec.y;
	newPos.z = this->getPosition().z + posVec.z;

	this->setPosition(newPos);
}

void Weakspot::updateSelf()
{
	//this->getRigidBody()->setLinearVelocity(btVector3(0.0f, 0.0f, 0.0f));
	whenImDead();
}

void Weakspot::whenImDead()
{
	bool amDead = this->isDead();
	if (amDead)
	{
		//reduce health of boss and change texture of this?
	}
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

void Weakspot::move(btVector3 velocityVec) //not used
{
	//this->getRigidBody()->setLinearVelocity(velocityVec);
}