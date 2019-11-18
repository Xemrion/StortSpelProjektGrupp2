#include "Boss.h"
#include "../States/PlayingGameState.h"

Boss::Boss()
{
	this->timeTilNextPoint = 0.0f;
	this->timeTilRotationChange = 0.0f;
	this->timeRotateDelay = 0;
	this->rotationVar = 0;
	this->currentPointNr = 0;

	this->currentPoint = { 0, 0, 0 };
	this->playerPos = { 0, 0, 0 };
	this->nrOfWeakpoints = 2;

	this->frontVector = Vector3{ 0, 0, 0 };
}

Boss::Boss(float x, float z, int weaponType, Physics* physics, std::vector<Weakspot*> &weakspots)
	:DynamicActor(x, z, physics), BossAblilities(&this->position, &this->targetPos, &this->velocity, weaponType, &this->deltaTime)
{
	this->setScale(Vector3(0.04f, 0.04f, 0.04f));
	this->setPosition(Vector3(position.x, position.y + 4.5f, position.z));
	setUpActor();
	Game::getGraphics().addToDraw(this);

	this->stats = VehicleStats::AIBoss;
	setHealth(this->stats.maxHealth);
	Game::getGraphics().loadModel("Entities/Roller_Melee");
	this->mesh = Game::getGraphics().getMeshPointer("Entities/Roller_Melee");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Roller_Melee"));

	this->attackRange = 35;

	this->timeTilNextPoint = 0.0f;
	this->timeTilRotationChange = 0.0f;
	this->rotationVar = 0;
	this->currentPointNr = 0;
	this->currentPoint = { 0, 0, 0 };
	this->playerPos = { 0, 0, 0 };

	this->weakSpots = weakspots;


	this->weakSpots[0]->setPosition(Vector3(0, 0, 0));
	this->weakSpots[1]->setPosition(Vector3 (0, 10, 0));
	
}

Boss::~Boss()
{
	Game::getGraphics().removeFromDraw(this);
}

void Boss::update(float dt, const Vector3& targetPos)
{
	Actor::update(dt, targetPos);
	this->movementVariables(dt);
	this->circulatePlayer(targetPos);

	this->updateBullets(dt);
}

void Boss::setUpActor()
{
	//Selectors
	this->root = &bt.getSelector();
	Selector& selector2 = bt.getSelector();
	Selector& selector3 = bt.getSelector();
	Selector& selector4 = bt.getSelector();
	Selector& selector5 = bt.getSelector();
	Selector& selector6 = bt.getSelector();
	Selector& selector7 = bt.getSelector();

	//Sequences
	Sequence& sequence1 = bt.getSequence();
	Sequence& sequence2 = bt.getSequence();
	Sequence& sequence3 = bt.getSequence();
	Sequence& sequence4 = bt.getSequence();
	Sequence& sequence5 = bt.getSequence();
	Sequence& sequence6 = bt.getSequence();

	//Range
	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Boss::inAttackRange, std::ref(*this)));

	//Attacks
	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Boss::shoot, std::ref(*this)));

	//Behavior& ability1 = bt.getAction();
	//ability1.addAction(std::bind(&BossAblilities::ability1, std::ref(*this)));

	//Behaviour Tree
	//attack and run,				selector
	root->addChildren(sequence1);
	root->addChildren(selector2);

	//check range					sequence
	sequence1.addChildren(inRange);
	sequence1.addChildren(shoot); //another sequence

	//abilities outside range		selector
	//selector2.addChildren(idle);

	//timer to switch weapon
}

void Boss::movementVariables(float dt)
{
	this->timeTilNextPoint += dt;
	this->timeTilRotationChange += dt;

	// Decides if boss rotates clock- or counter-clockwise
	if (this->timeTilRotationChange >= timeRotateDelay)
	{
		int randNr = rand() % 2;
		if (randNr == 0)
			this->rotationVar = 1;
		if (randNr == 1)
			this->rotationVar = -1;

		this->timeTilRotationChange = 0.0f;
		this->timeRotateDelay = (rand() % 3) + 2; //new delay
	}

	// Decides when boss moves to next point
	if (this->timeTilNextPoint >= 1.0f)
	{
		this->currentPointNr += this->rotationVar;

		if (this->currentPointNr == 8)
			this->currentPointNr = 0; //start over
		else if (this->currentPointNr == -1)
			this->currentPointNr = 7;

		this->timeTilNextPoint = 0.0f;
	}
}

void Boss::move()
{
	//To make the slow down not as abrupt
	acceleration *= 0.4f;
	// Update velocity
	velocity += acceleration;
	velocity += seek();
	// Limit speed
	if (velocity.Length() > maxForce)
	{
		velocity /= velocity.Length();
	}

	velocity *= 4.5;

	//move rigid body
	//Vector3 temp = position + Vector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime) * stats.maxSpeed;
	this->getRigidBody()->setLinearVelocity(btVector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime) * 200);
	this->currentVelocityVector = (btVector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime) * 200);
	Vector3 targetToSelf = (this->playerPos - position);
	//Rotate
	if ((targetToSelf).Dot(vecForward) < 0.8)
	{
		vecForward -= (targetToSelf * deltaTime) / 0.02f;
		vecForward.Normalize();
		this->frontVector = vecForward;

		float newRot = atan2(this->vecForward.x, this->vecForward.z);

		this->setRotation(Vector3(0, newRot - (DirectX::XM_PI / 2), 0));

		//update weakpoints
		this->updateWeakPoints(targetPos);
	}

	//position = temp;, DOESNT SET BODY ANYMORE, JUST MOVING THE RIGID BODY
	// Reset accelertion to 0 each cycle
	acceleration *= 0;
}

Vector3 Boss::seek()
{
	Vector3 desiredDirection;
	desiredDirection -= position - this->currentPoint;
	//desired *= maxSpeed;

	acceleration = desiredDirection - velocity;
	if (acceleration.Length() > maxForce)
	{
		acceleration /= acceleration.Length();
	}
	return acceleration;
}

void Boss::circulatePlayer(Vector3 targetPos)
{
	std::vector<Vector3> pointsAroundPlayer;
	Vector3 l_playerPos = targetPos;
	Vector3 radiusFromPlayer = { 12.5, 0, 12.5 };
	float division = 1.25;

	pointsAroundPlayer.push_back({ l_playerPos.x, l_playerPos.y, l_playerPos.z + radiusFromPlayer.z });
	pointsAroundPlayer.push_back({ l_playerPos.x + (radiusFromPlayer.x / division), l_playerPos.y, l_playerPos.z + (radiusFromPlayer.z / division) });
	pointsAroundPlayer.push_back({ l_playerPos.x + radiusFromPlayer.x, l_playerPos.y, l_playerPos.z });
	pointsAroundPlayer.push_back({ l_playerPos.x + (radiusFromPlayer.x / division), l_playerPos.y, l_playerPos.z - (radiusFromPlayer.z / division) });
	pointsAroundPlayer.push_back({ l_playerPos.x, l_playerPos.y, l_playerPos.z - radiusFromPlayer.z });
	pointsAroundPlayer.push_back({ l_playerPos.x - (radiusFromPlayer.x / division), l_playerPos.y, l_playerPos.z - (radiusFromPlayer.z / division) });
	pointsAroundPlayer.push_back({ l_playerPos.x - radiusFromPlayer.x, l_playerPos.y, l_playerPos.z });
	pointsAroundPlayer.push_back({ l_playerPos.x - (radiusFromPlayer.x / division), l_playerPos.y, l_playerPos.z + (radiusFromPlayer.z / division) });

	this->currentPoint = pointsAroundPlayer[currentPointNr];
	this->playerPos = targetPos;

	move();
}

void Boss::updateWeakPoints(Vector3 targetPos)
{
	Vector3 bossPos = this->getPosition();

	this->weakSpots[0]->startPos(bossPos);
	this->weakSpots[1]->startPos(bossPos);

	Vector3 upVector = { 0, 1, 0 };
	//frontvector is backwards in x and z for placement
	//Vector3 correctFrontVector = { (this->frontVector.x * -1), this->frontVector.y, (this->frontVector.z * -1) };
	Vector3 rightVector = XMVector3Cross(this->frontVector, upVector);

	float sideVar = 2.5;
	Vector3 sideOffset = Vector3((rightVector.x * sideVar), 0.0f, (rightVector.z * sideVar));
	Vector3 frontOffset = Vector3((-this->frontVector.x * 5.5), 0.0f, (-this->frontVector.z * 5.5));
	Vector3 heightOffset = Vector3(0, -4.5, 0);

	this->weakSpots[0]->shortMove(sideOffset);
	this->weakSpots[0]->shortMove(frontOffset); //moves y, should just move x and z
	this->weakSpots[0]->shortMove(heightOffset);


	this->weakSpots[1]->shortMove(-(sideOffset));
	this->weakSpots[1]->shortMove(frontOffset);
	this->weakSpots[1]->shortMove(heightOffset);

	//their own update
	for (int i = 0; i < this->nrOfWeakpoints; i++)
	{
		this->weakSpots[i]->updateSelf();
	}
}

//ADD WEAPONSWITCH, BOSSWEAPONS, MORE WEAPON SPOTS ON MESH
//MORE BOSS SPOTS THAT TAKE DAMAGE AND SHOOT, BOSS DEAD IF ALL DEAD
//HEALTHBAR
//behaviour tree