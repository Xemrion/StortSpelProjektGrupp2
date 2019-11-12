#include "Boss.h"
#include "../States/PlayingGameState.h"
Boss::Boss()
{
}

Boss::Boss(float x, float z, int weaponType)
	:DynamicActor(x, z), Ranged(&this->position, &this->targetPos, &this->velocity,&this->deltaTime, weaponType)
{
	this->setScale(Vector3(0.05f, 0.05f, 0.05f));
	setUpActor();
	Game::getGraphics().addToDraw(this);

	this->stats = VehicleStats::AIBoss;
	setHealth(this->stats.maxHealth);
	Game::getGraphics().loadModel("Entities/Roller_Melee");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Roller_Melee");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Roller_Melee"));

	this->attackRange = 10;
	this->testNr = 1;
}

Boss::~Boss()
{
	Game::getGraphics().removeFromDraw(this);
}

void Boss::update(float dt, Vector3 targetPos)
{
	DynamicActor::update(dt, targetPos);
	this->updateBullets(dt);

}

void Boss::setUpActor()
{
	this->root = &bt.getSelector();				//selector1
	Sequence& sequence1 = bt.getSequence();		//sequence1
	Selector& selector2 = bt.getSelector();		//selector2
	Sequence& sequence2 = bt.getSequence();		//sequence2

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Boss::inAttackRange, std::ref(*this)));

	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Boss::setChaseState, std::ref(*this)));

	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Boss::shoot, std::ref(*this)));

	Behavior& idle = bt.getAction();
	idle.addAction(std::bind(&Boss::setIdleState, std::ref(*this)));

	Behavior& circulate = bt.getAction();
	circulate.addAction(std::bind(&Boss::setCirculateState, std::ref(*this)));

	testNr++;
	//std::printf((char*)testNr);

	root->addChildren(sequence1);
	root->addChildren(chase);

	sequence1.addChildren(inRange);
	sequence1.addChildren(circulate);
}