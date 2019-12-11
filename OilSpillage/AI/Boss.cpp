#include "Boss.h"
#include "../States/PlayingGameState.h"

Boss::Boss()
{
	this->timeTilNextPoint = 0.0f;
	this->timeTilRotationChange = 0.0f;
	this->timeRotateDelay = 0;
	this->rotationVar = 0;
	this->phase = 1;
	this->scalingNr = 1;
	this->phaseShoot = this->phase;
	this->currentPointNr = 0;
	this->dt = 0;
	this->attackRange = 85;

	this->currentPoint = { 0, 0, 0 };
	this->playerPos = { 0, 0, 0 };
	
}

Boss::Boss(Boss&& boss)
{
	setUpActor();

	this->stats = VehicleStats::AIBoss;
	setHealth(this->stats.maxHealth);

	Game::getGraphics().loadModel("Entities/Boss");

	this->mesh = Game::getGraphics().getMeshPointer("Entities/Boss");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Boss"));
	this->setScale(Vector3(1, 1, 1));
	this->setPosition(Vector3(position.x, position.y, position.z));
	Game::getGraphics().addToDraw(this);


	this->attackRange = boss.attackRange;

	this->timeTilNextPoint = boss.timeTilNextPoint;
	this->timeTilRotationChange = boss.timeTilRotationChange;
	this->rotationVar = boss.rotationVar;
	this->phase = boss.rotationVar;
	this->scalingNr = boss.scalingNr;
	this->phaseShoot = this->phase;
	this->currentPointNr = boss.currentPointNr;
	this->currentPoint = boss.currentPoint;
	this->playerPos = boss.playerPos;
	this->dt = boss.dt;

	//weakspots
	this->weakSpots.push_back(boss.weakSpots[0]);
	this->weakSpots.push_back(boss.weakSpots[1]);
	this->setPoints(2000 * (1 + (0.2 * Game::getGameInfo().nrOfClearedStages)));
}

Boss::Boss(float x, float z, int weaponType, Physics* physics, float scalingNr)
	:DynamicActor(x, z, physics), BossAbilities(&this->position, &this->targetPos, &this->velocity, weaponType)
{
	setUpActor();

	this->stats = VehicleStats::AIBoss;
	setHealth(this->stats.maxHealth);

	Game::getGraphics().loadModel("Entities/Boss");

	this->mesh = Game::getGraphics().getMeshPointer("Entities/Boss");
	this->setMaterial(Game::getGraphics().getMaterial("Entities/Boss"));
	this->setScale(Vector3(1, 1, 1));
	this->setPosition(Vector3(position.x, position.y, position.z));
	Game::getGraphics().addToDraw(this);

	this->attackRange = 85;

	this->timeTilNextPoint = 0.0f;
	this->timeTilRotationChange = 0.0f;
	this->rotationVar = 0;
	this->phase = 1;
	this->scalingNr = scalingNr;
	this->phaseShoot = this->phase;
	this->currentPointNr = 0;
	this->currentPoint = { 0, 0, 0 };
	this->playerPos = { 0, 0, 0 };
	this->dt = 0;


	//weakpoints
	this->weakSpots.push_back(Weakspot(0, this->scalingNr));
	this->weakSpots.push_back(Weakspot(0, this->scalingNr));
	this->initiateWeakPoints();

	btRigidBody* tempo = physics->addSphere(3.5f, btVector3(position.x, position.y, position.z), 1.5f, this);
	setRigidBody(tempo);
	getRigidBody()->activate();
	getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	getRigidBody()->setFriction(0);
	getRigidBody()->setLinearFactor(btVector3(1, 0, 1));
}

Boss::~Boss()
{
	Game::getGraphics().removeFromDraw(this);
	if (this->isDead())
	{
		Game::getGameInfo().nrOfBosses++;
	}

}

void Boss::update(float dt, const Vector3& targetPos)
{
	this->dt = dt;
	Actor::update(dt, targetPos);
	this->movementVariables(dt);
	this->circulatePlayer(targetPos);
	onFire();

	if (this->phase == 1)
		this->updateWeakPoints();
	else if (this->phase == 2)
	{
		this->enterPhase2();
	}

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

	//Wait
	Behavior& wait = bt.getAction();
	wait.addAction(std::bind(&Boss::waitForStart, std::ref(*this)));

	//Other actions
	Behavior& switchWeapon = bt.getAction();
	switchWeapon.addAction(std::bind(&Boss::switchWeapon, std::ref(*this)));
	
	//Check Range
	Behavior& inRange = bt.getAction();
	inRange.addAction(std::bind(&Boss::inAttackRange, std::ref(*this)));

	//Attacks
	Behavior& shoot = bt.getAction();
	shoot.addAction(std::bind(&Boss::shoot, std::ref(*this)));

	Behavior& ability1 = bt.getAction();
	ability1.addAction(std::bind(&Boss::ability1, std::ref(*this)));

	//Behaviour Tree
	//attack and run,				selector
	root->addChildren(sequence1);
	//root->addChildren(selector2);

	//sequence2.addChildren(wait); //another sequence //wait works

	sequence1.addChildren(inRange);
	sequence1.addChildren(selector2); //another sequence
	
	selector2.addChildren(sequence2); //needs to be sequence -> selector  -> sequence etc.

	sequence2.addChildren(shoot);
	sequence2.addChildren(switchWeapon);

	//sequence2.addChildren(shoot);
	//sequence2.addChildren(wait); //another sequence

	

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
	if (this->phase == 1)
	{
		if (this->timeTilNextPoint >= 1.1f)
		{
			this->currentPointNr += this->rotationVar;

			if (this->currentPointNr == 8)
				this->currentPointNr = 0; //start over
			else if (this->currentPointNr == -1)
				this->currentPointNr = 7;

			this->timeTilNextPoint = 0.0f;
		}
	}
	else if (this->phase == 2)
	{
		if (this->timeTilNextPoint >= 0.65f)
		{
			this->currentPointNr += this->rotationVar;

			if (this->currentPointNr == 8)
				this->currentPointNr = 0; //start over
			else if (this->currentPointNr == -1)
				this->currentPointNr = 7;

			this->timeTilNextPoint = 0.0f;
		}
	}
}

void Boss::move()
{
	velocity += calculateVelocity();
	velocity.Normalize();

	if (this->phase == 1)
		velocity *= 4.25;
	else if (this->phase == 2)
		velocity *= 6.5;

	//move rigid body
	//Vector3 temp = position + Vector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime) * stats.maxSpeed;
	this->getRigidBody()->setLinearVelocity(btVector3(velocity.x * 0.01, 0.0f, velocity.z * 0.01) * 200);

	this->currentVelocityVector = (btVector3(velocity.x * deltaTime, 0.0f, velocity.z * deltaTime) * 200);
	Vector3 targetToSelf = (this->playerPos - position);
	this->targetToSelf = targetToSelf;

	//Rotate
	if ((targetToSelf).Dot(vecForward) < 0.8)
	{
		vecForward -= (targetToSelf * deltaTime) / 0.02f;
		vecForward.Normalize();
		this->frontVecShoot = vecForward;

		float newRot = atan2(this->vecForward.x, this->vecForward.z);

		this->setRotation(Vector3(0, (newRot - (DirectX::XM_PI / 2)), 0));

		if (this->weakSpots.size() > 0)
		{
			for (int i = 0; i < this->weakSpots.size(); i++)
			{
				weakSpots[i].setRotation(Vector3(0, (newRot - (DirectX::XM_PI / 2)), 0));
			}
		}
	}

	//position = temp;, DOESNT SET BODY ANYMORE, JUST MOVING THE RIGID BODY
}

Vector3 Boss::calculateVelocity()
{
	Vector3 desiredDirection = this->currentPoint - position;

	return desiredDirection - velocity;
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

void Boss::enterPhase2()
{
	//should only be done once
	this->phaseShoot = this->phase;
	this->switchWeapon();
	this->stats = VehicleStats::AIBossWeak;
	this->stats.maxHealth *= this->scalingNr;
}

void Boss::initiateWeakPoints()
{
	int yPos = 0;
	for (int i = 0; i < this->weakSpots.size(); i++)
	{
		this->weakSpots[i].setPosition(Vector3(0, yPos, 0));
		yPos += 10;

		this->weakSpots[i].setWeakspotNr(i);
	}
}

void Boss::updateWeakPoints()
{
	//lower red color from hit
	for (int i = 0; i < this->weakSpots.size(); i++) // checks to remove red
	{
		if (this->weakSpots[i].getIsHit())
		{
			this->weakSpots[i].setColor(Vector4(
				max(this->weakSpots[i].getColor().x / (1 + (15.0f * this->dt)), 0), 
				this->weakSpots[i].getColor().y,
				this->weakSpots[i].getColor().z,
				1));
			if (this->weakSpots[i].getColor().x <= 0.01f)
			{
				this->weakSpots[i].setIsHit(false); //stop lowering redness
			}
		}
	}

	if (this->weakSpots.size() > 0)
	{
		Vector3 bossPos = this->getPosition();

		for (int i = 0; i < this->weakSpots.size(); i++)
		{
			this->weakSpots[i].startPos(bossPos);
		}

		//Vectors for position
		Vector3 upVector = { 0, 1, 0 };
		Vector3 rightVector = XMVector3Cross(this->vecForward, upVector);
		this->rightVecShoot = rightVector;

		//make the offsets
		float sideVar = 2.5;
		Vector3 sideOffset = Vector3((rightVector.x * sideVar), 0.0f, (rightVector.z * sideVar));
		Vector3 frontOffset = Vector3((-this->vecForward.x * 4.0), 0.0f, (-this->vecForward.z * 4.0));
		Vector3 heightOffset = Vector3(0, 0.3f, 0);

		//update position
		for (int i = 0; i < this->weakSpots.size(); i++)
		{
			if (this->weakSpots[i].getWeakspotNr() == 0)
			{
				this->weakSpots[i].shortMove(sideOffset);
				this->weakSpots[i].shortMove(frontOffset);
				this->weakSpots[i].shortMove(heightOffset);
			}
			else if (this->weakSpots[i].getWeakspotNr() == 1)
			{
				this->weakSpots[i].shortMove(-sideOffset);
				this->weakSpots[i].shortMove(frontOffset);
				this->weakSpots[i].shortMove(heightOffset);
			}
		}
	}
	else if (this->weakSpots.size() == 0)
		this->phase = 2;
}

void Boss::checkIfWeakPointHit(Bullet* bulletArray, size_t size, float &soundTimer)
{
	for (int i = 0; i < this->weakSpots.size(); i++)
	{
		for (int j = 0; j < size; j++)
		{
			if (bulletArray[j].getGameObject()->getAABB().intersectXZ(this->weakSpots[i].getAABB()))
			{
				if (soundTimer > 0.05f)
				{
					Sound::play("HitSound.wav");
					soundTimer = 0;
				}

				this->weakSpots[i].changeHealth(bulletArray[j].getDamage(), deltaTime);
				bulletArray[j].destroy();
			}
		}
	}

	for (int i = 0; i < this->weakSpots.size(); i++)
	{
		//check if any weakpoint died
		if (this->weakSpots[i].getDead())
		{
			this->weakSpots[i] = this->weakSpots.back();
			this->weakSpots.pop_back();
			i--;
		}
	}
}

void Boss::changeHealth(float amount)
{
	if (amount < 0)
		this->isHit = true;

	if (this->phase == 1)
	{

	}
	else if (this->phase == 2)
	{
		setColor(Vector4(max(getColor().x + -amount * 0.1f, 0), getColor().y, getColor().z, 1));
		this->setHealth(std::clamp(this->getHealth() + amount, 0.0f, (this->stats.maxHealth * this->scalingNr)));
		Game::getGraphics().addParticle2(this->getPosition(), Vector3(0, 0, 0), 1, 0.5f);
	}
}

std::vector<Weakspot>& Boss::getWeakspots()
{
	return this->weakSpots;
}


//spin when entering phase 2
//constrcutor gets nr of stages cleared och scales from that / 3 sains every 3 level is boss