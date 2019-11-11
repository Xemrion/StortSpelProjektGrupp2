#include "Boss.h"
#include "../States/PlayingGameState.h"
Boss::Boss()
{
}

Boss::Boss(float x, float z, int weaponType)
	:DynamicActor(x, z), Ranged(&this->position, &this->targetPos, &this->velocity, weaponType)
{
	this->setScale(Vector3(0.05f, 0.05f, 0.05f));
	setUpActor();
	Game::getGraphics().addToDraw(this);

	this->stats = VehicleStats::AIBoss;
	setHealth(this->stats.maxHealth);
	Game::getGraphics().loadModel("Entities/Roller_Melee");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Roller_Melee");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Roller_Melee"));
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
	this->root = &bt.getSelector();
	Sequence& sequence = bt.getSequence();
	Selector& selector = bt.getSelector();
	Sequence& seq2 = bt.getSequence();

	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Boss::inAttackRange, std::ref(*this)));

	Behavior& chase = bt.getAction();
	chase.addAction(std::bind(&Boss::setChaseState, std::ref(*this)));

	Behavior& roam = bt.getAction();
	roam.addAction(std::bind(&Boss::setIdleState, std::ref(*this)));

	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Boss::shoot, std::ref(*this)));

	Behavior& inAggroRange = bt.getAction();
	inAggroRange.addAction(std::bind(&Boss::inAggroRange, std::ref(*this)));

	root->addChildren(sequence);
	root->addChildren(roam);

	sequence.addChildren(inAggroRange);
	sequence.addChildren(selector);

	selector.addChildren(seq2);
	selector.addChildren(chase);

	seq2.addChildren(inRange);
	seq2.addChildren(shoot);
}