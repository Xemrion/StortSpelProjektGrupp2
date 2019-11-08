#include "Vehicle.h"
#include"game.h"
#include "Input.h"
#include "States/PlayingGameState.h"

Vehicle::Vehicle()
{
	velocity = Vector2(0.0f, 0.0f);
	accelerator = Vector3(0.0f, 0.0f, 0.0f);
	acceleratorTempX = 0;
	acceleratorTempZ = 0;
	strength = 0;
	this->dmg = false;
	this->deadImpulse = false;
	this->respawnTimer = 0.0f;
	this->totRespawnTime = 5.0f;
	this->immortalTimer = 0.0f;
	this->immortal = false;

	targetRotation = 0.0f;
	drivingMode = 0;
	topSpeed = 4700;
	this->add = 0.0f;
	this->counter = 0.0f;
	this->rotateAcceleration = 0.0f;
	this->rotationSmoother = 1.0f;
	this->tempTargetRotation = 0.0f;
	this->velocitySimple = 0.0f;
	this->velocitySpeed = 0.0f;
	this->rotationDirection = true;
	this->vehicle = nullptr;
	this->cameraDistance = 0.0f;
	this->accelForce = Vector3(0,0,0);
	this->reverseTimer = 0;
	this->reverseTimer2 = 0;

	this->curDir = Vector2(0.0f, 1.0f);//UP
	this->timeSinceLastShot = 0.0f;
	this->timeSinceLastShot2 = 0.0f;

	this->weapon = WeaponHandler::getWeapon(WeaponType::MachineGun);
	this->weapon2 = WeaponHandler::getWeapon(WeaponType::Flamethrower);
	this->defaultStats = VehicleStats::fastCar;
	this->updatedStats = this->defaultStats;
	this->health = this->updatedStats.maxHealth;

	this->vehicleSlots = nullptr;
	
}

Vehicle::~Vehicle()
{
	delete vehicle;
	delete vehicleBody1;

	delete this->mountedWeapon;
	delete this->frontWeapon;
	delete wheel1;
	delete wheel2;
	delete wheel3;
	delete wheel4;
	delete spring1;
	delete pointJoint;
}

void Vehicle::init(Physics *physics)
{
	this->physics = physics;
	this->mountedWeapon = new GameObject;
	this->frontWeapon = new GameObject;

	this->vehicle = new GameObject;
	Game::getGraphics().loadShape(Shapes::SHAPE_CUBE);
	vehicle->mesh = Game::getGraphics().getMeshPointer("Cube");
	//Game::getGraphics().addToDraw(vehicle);
	vehicle->setPosition(Vector3(0.0f, 10.0f, 0.0f));
	vehicle->setScale(Vector3(0.5f, 0.14f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp");
	vehicle->setTexture(Game::getGraphics().getTexturePointer("CarTemp"));
	Game::getGraphics().loadModel("Entities/Dummy_Turret");
	mountedWeapon->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Turret1");
	Material mountedWPMaterial = Game::getGraphics().getMaterial("Entities/Dummy_Turret");
	mountedWeapon->setTexture(mountedWPMaterial.diffuse);
	Game::getGraphics().addToDraw(mountedWeapon);
	mountedWeapon->setScale(Vector3(0.010f,0.007f, 0.007f));

	frontWeapon->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Turret1");
	Material frontWeaponMat = Game::getGraphics().getMaterial("Entities/Dummy_Turret");
	frontWeapon->setTexture(frontWeaponMat.diffuse);
	Game::getGraphics().addToDraw(frontWeapon);
	frontWeapon->setScale(Vector3(0.002f, 0.005f, 0.005f));
	frontWeapon->setColor(Vector4(0.7f, 0.7f, 0.0f, 1.0f));

	this->vehicleBody1 = new GameObject;
	vehicleBody1->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Player_Car1");
	vehicleBody1->setSpotShadow(false);
	Game::getGraphics().addToDraw(vehicleBody1);
	vehicleBody1->setScale(Vector3(0.005f, 0.005f, 0.005f));	
	//vehicleBody1->setPosition(Vector3(0.0f, 0.65f, 0.0f));
	//vehicleBody1->setScale(Vector3(0.5f, 0.22f, 0.9f));

	vehicleBody1->setTexture(Game::getGraphics().getMaterial("Entities/Dummy_Player_Car").diffuse);

	this->wheel1 = new GameObject;
	this->wheel2 = new GameObject;
	this->wheel3 = new GameObject;
	this->wheel4 = new GameObject;
	wheel1->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Player_Car");
	wheel2->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Player_Car2");
	wheel3->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Player_Car3");
	wheel4->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Player_Car4");
	Game::getGraphics().addToDraw(wheel1);
	Game::getGraphics().addToDraw(wheel2);
	Game::getGraphics().addToDraw(wheel3);
	Game::getGraphics().addToDraw(wheel4);
	wheel1->setPosition(Vector3(0.0f, 1.0f, 0.0f));
	wheel2->setPosition(Vector3(0.0f, 1.0f, 0.0f));
	wheel3->setPosition(Vector3(0.0f, 1.0f, 0.0f));
	wheel4->setPosition(Vector3(0.0f, 1.0f, 0.0f));
	wheel1->setScale(Vector3(0.005f, 0.005f, 0.005f));
	wheel2->setScale(Vector3(0.005f, 0.005f, 0.005f));
	wheel3->setScale(Vector3(0.005f, 0.005f, 0.005f));
	wheel4->setScale(Vector3(0.005f, 0.005f, 0.005f));
	wheel1->setTexture(Game::getGraphics().getMaterial("Entities/Dummy_Player_Car").diffuse);
	wheel2->setTexture(Game::getGraphics().getMaterial("Entities/Dummy_Player_Car").diffuse);
	wheel3->setTexture(Game::getGraphics().getMaterial("Entities/Dummy_Player_Car").diffuse);
	wheel4->setTexture(Game::getGraphics().getMaterial("Entities/Dummy_Player_Car").diffuse);


	btRigidBody* tempo = physics->addBox(btVector3(vehicle->getPosition().x, vehicle->getPosition().y, vehicle->getPosition().z), btVector3(vehicle->getScale().x, vehicle->getScale().y, vehicle->getScale().z), 10.0f);
	this->getVehicle()->setRigidBody(tempo, physics);
	this->getVehicle()->getRigidBody()->activate();
	this->getVehicle()->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	this->getVehicle()->getRigidBody()->setFriction(0);
	this->getVehicle()->getRigidBody()->setLinearFactor(btVector3(1,0,1));

	tempo = physics->addBox(btVector3(vehicle->getPosition().x, vehicle->getPosition().y + 0.65f, vehicle->getPosition().z), -btVector3(this->vehicleBody1->getAABB().minPos.x - this->vehicleBody1->getAABB().maxPos.x, (this->vehicleBody1->getAABB().minPos.y - this->vehicleBody1->getAABB().maxPos.y) * 0.2f, this->vehicleBody1->getAABB().minPos.z - this->vehicleBody1->getAABB().maxPos.z) * 0.5f, 1.0f);
	vehicleBody1->setRigidBody(tempo, physics);
	vehicleBody1->getRigidBody()->activate();
	vehicleBody1->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	vehicleBody1->getRigidBody()->setFriction(1);

	/*spring1 = physics->addSpring(this->vehicle->getRigidBody(),this->wheel1->getRigidBody());
	spring1->setLinearLowerLimit(btVector3(0.0f, 0.0f, 0.0f));
	spring1->setLinearUpperLimit(btVector3(0.0f, 0.56f, 0.0f));
	spring1->enableSpring(1, true);
	spring1->setStiffness(1, 65.0f);
	spring1->setDamping(1, 0.5f);
	spring1->setEquilibriumPoint();*/
	
	pointJoint = physics->addPointJoint(this->vehicle->getRigidBody(), this->vehicleBody1->getRigidBody());
	//vehicleBody1->getRigidBody()->setDamping(btScalar(0),3);
	test = *this->mountedWeapon;
	Game::getGraphics().addToDraw(&test);
	test.setPosition(Vector3(2, 0, 0));
}

void Vehicle::update(float deltaTime)
{
	if (dynamic_cast<PlayingGameState*>(Game::getCurrentState()) != nullptr) {
		if (this->deadImpulse == true && this->health <= 0)
			{
				this->respawnTimer += deltaTime;
				if(this->respawnTimer>5)
				{
					this->resetHealth();
					this->immortal = true;
					this->vehicle->setPosition(dynamic_cast<PlayingGameState*>(Game::getCurrentState())->getRespawnPosition());
					this->vehicleBody1->setPosition(dynamic_cast<PlayingGameState*>(Game::getCurrentState())->getRespawnPosition() + Vector3(0.0f, 0.65f, 0.0f));

					this->deadImpulse = false;
					this->respawnTimer = 0.0f;
				}
			}
		if (dynamic_cast<PlayingGameState*>(Game::getCurrentState())->getTime() == 0)
		{
			this->respawnTimer = 0.0f;
		}
	}
	
	for (int i = 0; i < (int)PowerUpType::Length; ++i)
	{
		powerUpTimers[i] = max(powerUpTimers[i] - deltaTime, 0.0f);
	}
	if (powerUpTimers[(int)PowerUpType::Speed] > 0.0)
	{
		this->updatedStats.accelerationRate = this->defaultStats.accelerationRate * 3.0f;
		this->vehicle->setColor(Vector4(0.0, 1.0, 0.0, 1.0));
	}
	else
	{
		this->updatedStats.accelerationRate = this->defaultStats.accelerationRate;
		this->vehicle->setColor(Vector4(0.0, 0.0, 0.0, 1.0));
	}
	
	tempTargetRotation = targetRotation;

	//Quaternion Rotation to Euler
	btScalar m;
	btScalar n;
	Quaternion qt = Vector4(this->vehicle->getRigidBody()->getWorldTransform().getRotation().getX(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getY(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getZ(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getW());

	qt = this->vehicle->getRotationQuaternion();

	float heading = getHeading(qt);

	this->vehicle->getRigidBody()->getWorldTransform().getRotation().getEulerZYX(n,mRotation,m);

	//Current rotation of vehicle
	float vehicleRotation = fmod((/*(getYaw(qt)*/heading * 180 / DirectX::XM_PI), 360.0f);
	if (vehicleRotation < 0)
		vehicleRotation += 360;
	mRotation = vehicleRotation;

	//Rotate Chassis toward Body
	vehicleBody1->getRigidBody()->applyImpulse(btVector3(0,80*deltaTime,0),btVector3(0,10,0));
	vehicleBody1->getRigidBody()->setDamping(0,0.99f);
	qt = Vector4(this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getX(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getY(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getZ(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getW());
	float bodyHeading = getHeading(qt);

	bodyHeading = fmod((bodyHeading * 180 / DirectX::XM_PI), 360.0f);
	heading = fmod((heading * 180 / DirectX::XM_PI), 360.0f);
	float difference = (180 - abs(abs(bodyHeading - heading) - 180))*0.45f; ;
	if (bodyHeading < heading) {
		if (abs(bodyHeading - heading) < 180) {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}else {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), -difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}
	}else {
		if (abs(bodyHeading - heading) < 180) {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), -difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}else {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}
	}

	//Direction of Vehicle in dx & dy
	float dx = sin((DirectX::XM_PI / 180) * vehicleRotation);
	float dy = -cos((DirectX::XM_PI / 180)* vehicleRotation);


	targetRotation = (atan2(-Input::getDirectionL().x, -Input::getDirectionL().y) * 180 / DirectX::XM_PI) + 180;
	velocitySpeed = (this->vehicle->getRigidBody()->getLinearVelocity().getX() * (dx)) + (-this->vehicle->getRigidBody()->getLinearVelocity().getZ() * (dy));
	
	Vector3 steering = Vector3(vehicle->getRigidBody()->getAngularVelocity().getX(),
		Input::getStrengthL() * /*deltaTime*/0.035f * 80 * min(velocitySpeed * 0.15f, 1),
		vehicle->getRigidBody()->getAngularVelocity().getZ());

	Vector3 steering2 = Vector3(vehicle->getRigidBody()->getAngularVelocity().getX(),
		Input::getStrengthL() * /*deltaTime*/0.035f * 80 * max(velocitySpeed * 0.15f, -1),
		vehicle->getRigidBody()->getAngularVelocity().getZ());
	
	//Driving mode: Throttle and turning, realistic
	if (drivingMode == 0) {
		if ((Input::checkButton(Keys::R_TRIGGER, States::HELD) || Input::isKeyDown_DEBUG(Keyboard::W))&&this->health>0) {
			if (velocitySpeed < (40 * updatedStats.maxSpeed)) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(dx* deltaTime * 160.0f * updatedStats.accelerationRate, 0, -(dy * deltaTime * 160.0f * updatedStats.accelerationRate)), btVector3(0, 0, 0));
			}
		}
		if (Input::getDirectionL().x > 0 && this->health > 0) {
			if (velocitySpeed > 0.0f) {
				if (rotationDirection == true) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 10 * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL();
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL()) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL();
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, steering.y * min(rotateAcceleration, 1), steering.z));
				rotationDirection = false;
			}
			else {
				if (rotationDirection == false) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 10 * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL();
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL()) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL();
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, steering2.y/* * min(rotateAcceleration, 1)*/, steering.z));
				rotationDirection = true;
			}
		}
		else if (Input::getDirectionL().x < 0 && this->health>0) {
			if (velocitySpeed > 0.0f) {
				if (rotationDirection == true) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 10 * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL();
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL()) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL();
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, -steering.y * min(rotateAcceleration, 1), steering.z));
				rotationDirection = false;
			}
			else {
				if (rotationDirection == false) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 10 * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL();
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL()) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * Input::getStrengthL();
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, -steering2.y/* min(rotateAcceleration,1)*/, steering.z));
				rotationDirection = true;
			}
		}
		else {
			rotateAcceleration = 0;
			rotateAcceleration /= 1.0f + 20 * deltaTime;
			if (health > 0) {
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, vehicle->getRigidBody()->getAngularVelocity().getY() / (1 + (7.0f * deltaTime)), steering.z));
			}
			else {
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, (vehicle->getRigidBody()->getAngularVelocity().getY() / (1 + (0.8f * deltaTime)))/** min(abs(velocitySpeed), 1)*/, steering.z));
			}

		}
		if ((Input::checkButton(Keys::L_TRIGGER, States::HELD) || Input::isKeyDown_DEBUG(Keyboard::S)) && velocitySpeed > (-40 * updatedStats.maxSpeed) && this->health>0) {
			this->vehicle->getRigidBody()->applyImpulse(btVector3(-(dx* deltaTime * 160.0f * 0.7f * updatedStats.accelerationRate), 0, (dy * deltaTime * 160.0f * 0.7f * updatedStats.accelerationRate)), btVector3(0, 0, 0));

			if (velocitySpeed > 0.0f) {
				vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (1.8f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (1.8f * deltaTime))));
			}
		}
		if (!Input::checkButton(Keys::R_TRIGGER, States::HELD) && !Input::isKeyDown_DEBUG(Keyboard::S) && !Input::checkButton(Keys::L_TRIGGER, States::HELD) && !Input::isKeyDown_DEBUG(Keyboard::W)) {
			vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (0.6f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (0.6f * deltaTime))));
		}
	}
	//Driving Mode: Turn towards direction, semi-realistic
	else if (drivingMode == 1) {

		Vector3 steering3 = Vector3(vehicle->getRigidBody()->getAngularVelocity().getX(),
			/*deltaTime*/0.035f * 80 * min(velocitySpeed * 0.15f, 1),
			vehicle->getRigidBody()->getAngularVelocity().getZ());

		if (Input::getStrengthL() > 0 && this->health>0) {
			if (velocitySpeed < 0.5f) {
				reverseTimer += 10.0f * deltaTime;
			}
			else {
				reverseTimer = 0;
				reverseTimer2 = 0;
			}
			if ((reverseTimer > 10.0f) && !(reverseTimer2 > 10.0f)) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(-dx* deltaTime * 160.0f * updatedStats.accelerationRate, 0, (dy * deltaTime * 160.0f * updatedStats.accelerationRate))*Input::getStrengthL(), btVector3(0, 0, 0));
				reverseTimer2 += 10.0f * deltaTime;
				if (reverseTimer2 > 10.0f) {
					reverseTimer = 0;
					reverseTimer2 = 0;
				}
			}
			else {
				reverseTimer2 = 0;
				if (velocitySpeed < (40 * updatedStats.maxSpeed)) {
					this->vehicle->getRigidBody()->applyImpulse(btVector3(dx * deltaTime * 160.0f * updatedStats.accelerationRate, 0, -(dy * deltaTime * 160.0f * updatedStats.accelerationRate))* Input::getStrengthL(), btVector3(0, 0, 0));
				}
			}

			if (strength < Input::getStrengthL()) {
				strength = Input::getStrengthL();
			}
			else {
				strength -= 0.02f * deltaTime;
			}

			float difference2 = min((180 - abs(abs(vehicleRotation - targetRotation) - 180)) * 0.05f,1.0f);

			if (vehicleRotation < targetRotation) {
				if (abs(vehicleRotation - targetRotation) < 180) {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, steering3.y* difference2, 0));
				}
				else {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, -steering3.y * difference2, 0));
				}
			}
			else {
				if (abs(vehicleRotation - targetRotation) < 180) {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, -steering3.y * difference2, 0));
				}
				else {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, steering3.y* difference2, 0));
				}
			}
		}
		else if(health > 0) {
			vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (1.8f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (1.8f * deltaTime))));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 40;
			//vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime*2 * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
			vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 120 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
		}
		else {
			vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (0.6f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (0.6f * deltaTime))));
			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 40;
			
			vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, (vehicle->getRigidBody()->getAngularVelocity().getY() / (1 + (0.8f * deltaTime)))/** min(abs(velocitySpeed), 1)*/, steering.z));
		}

		rotateAcceleration /= 1.0f + 6*deltaTime;
	}


	//Drifting
	float hypoC = sqrt(pow(dx, 2) + (pow(dy, 2)));
	float driftForce = this->vehicle->getRigidBody()->getLinearVelocity().getX() * (dy / hypoC) + -this->vehicle->getRigidBody()->getLinearVelocity().getZ() * -(dx / hypoC);
	Vector2 driftResistance = Vector2(-((dy / hypoC) * 4000 * deltaTime) * updatedStats.handlingRate, -(-((dx / hypoC) * 4000 * deltaTime)) * updatedStats.handlingRate);
	if (abs(driftForce) < 250) {
		driftResistance = driftResistance * (abs(driftForce)*0.005f);
	}
	if (!(health > 0)) {
		//driftResistance /= 2;
	}
	if (drivingMode != 2) {
		if (Input::getStrengthL() > 0) {
			if (driftForce < -0) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(-driftResistance.x,0,0),btVector3(0,0,0));
				this->vehicle->getRigidBody()->applyImpulse(btVector3(0, 0, driftResistance.y), btVector3(0, 0, 0));
			}
			else if (driftForce > 0) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(driftResistance.x, 0, 0), btVector3(0, 0, 0));
				this->vehicle->getRigidBody()->applyImpulse(btVector3(0, 0, -driftResistance.y), btVector3(0, 0, 0));
			}
		}
		else {
			if (driftForce < -0) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(-driftResistance.x, 0, 0), btVector3(0, 0, 0));
				this->vehicle->getRigidBody()->applyImpulse(btVector3(0, 0, driftResistance.y), btVector3(0, 0, 0));
			}
			else if (driftForce > 0) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(driftResistance.x, 0, 0), btVector3(0, 0, 0));
				this->vehicle->getRigidBody()->applyImpulse(btVector3(0, 0, -driftResistance.y), btVector3(0, 0, 0));
			}
		}
	}
	//this->vehicle->getRigidBody()->setLinearVelocity(btVector3((velocity.x * deltaTime*2 * 0.002f) * 100.0f, this->vehicle->getRigidBody()->getLinearVelocity().getY(), -(velocity.y * deltaTime*2 * 0.002f) * 100.0f));
	vehicle->getRigidBody()->setAngularVelocity(btVector3(0, vehicle->getRigidBody()->getAngularVelocity().getY(), 0));


	if (deltaTime > 0.01f) {
		soundTimer += 100.0f * deltaTime;
	}
	else {
		soundTimer += 100.0f;
	}

	add += 0.4f * deltaTime;

	if (dmg == true) {
		vehicleBody1->setColor(Vector4(vehicleBody1->getColor().x / (1 + 15.0f * deltaTime), vehicleBody1->getColor().y , vehicleBody1->getColor().z, 1));
		if (vehicleBody1->getColor().x <= 0.01f) {
			dmg = false;
		}
	}

	if (this->immortal)
	{
		this->immortalTimer += deltaTime;
		float sine = sin((this->immortalTimer - 0) / 0.3f) - 0.5f;
		float color = max(sine, 0.0f);
		this->vehicleBody1->setColor(Vector4(color, color, color, color));
		if (this->immortalTimer > 5)
		{
			this->immortalTimer = 0.0f;
			this->immortal = false;
		}
	}
}

void Vehicle::updateWeapon(float deltaTime)
{
	if (this->vehicleSlots != nullptr)
	{


		/*MOUNTED*/
		this->vehicleSlots->getSlot(Slots::MOUNTED)->getObject()->setPosition(this->vehicleBody1->getPosition());

		/*END*/

		/*FRONT*/
		Vector3 frontTempDir = Vector3(cos(this->vehicleBody1->getRotation().y - 3.14 / 2), 0, -sin(this->vehicleBody1->getRotation().y - 3.14 / 2));
		float yaw = this->vehicleBody1->getRotation().y;
		float pitch = this->vehicleBody1->getRotation().z;
		Vector3 tempDirYawPtich = Vector3(cos(yaw) * cos(pitch), sin(yaw) * cos(pitch), sin(pitch));
		tempDirYawPtich.Normalize();
		//this->frontWeapon->setPosition(this->vehicleBody1->getPosition() + tempDirYawPtich * 1 - Vector3(0, 1, 0));
		float angleWP = tempDirYawPtich.Dot(Vector3(0, 0, 1));
		this->vehicleSlots->getSlot(Slots::FRONT)->getObject()->setRotation(Vector3(0, this->vehicleBody1->getRotation().y + 3.14 / 2, acos(angleWP) - 3.14 / 2));
		this->vehicleSlots->getSlot(Slots::FRONT)->getObject()->setPosition(this->vehicleBody1->getPosition() + 1.15f * frontTempDir - Vector3(0.0f, 1.0f, 0.0f));

		/*END*/

		/*BACK*/
		this->vehicleSlots->getSlot(Slots::BACK)->getObject()->setPosition(this->vehicleBody1->getPosition() - 1.15f * frontTempDir - Vector3(0.0f, 1.0f, 0.0f));
		this->vehicleSlots->getSlot(Slots::BACK)->getObject()->setRotation(Vector3(0, this->vehicleBody1->getRotation().y - 3.14 / 2, acos(angleWP) - 3.14 / 2));

		/*END*/

		//this->frontWeapon->setRotation(Vector3(0, this->vehicleBody1->getRotation().y+3.14/2, ));
		Vector2 dir = Input::getDirectionR();
		dir.Normalize();
		if ((dir - curDir).Length() > 0.01f)
		{
			float l = dir.Dot(curDir);
			l = acos(l);
			if (l < XM_PI + 0.1f && l > XM_PI - 0.1f)
			{
				dir.Normalize();
				curDir = Vector2(0.5f + curDir.x * 0.5f, 0.5f + curDir.y * 0.5f);
			}
			curDir = Vector2::Lerp(curDir, dir, deltaTime * 20);
			curDir.Normalize();
		}
		else
		{
			curDir = dir;
		}

		if (this->health > 0)
		{
			float newRot = atan2(curDir.x, curDir.y) + 3.14f / 2;
			this->gunRotation = newRot;
			this->vehicleSlots->getSlot(Slots::FRONT)->getObject()->setRotation(Vector3(0, newRot, 0));

			this->timeSinceLastShot += deltaTime;
			this->timeSinceLastShot2 += deltaTime;

			// recoil goes from 100% to 0% in half a second
			this->weapon.currentSpreadIncrease = max(this->weapon.currentSpreadIncrease - deltaTime * this->weapon.maxSpread * 2.0, 0.0);
			this->weapon2.currentSpreadIncrease = max(this->weapon.currentSpreadIncrease - deltaTime * this->weapon.maxSpread * 2.0, 0.0);
			if (dynamic_cast<PlayingGameState*>(Game::getCurrentState()) != nullptr)
			{
				if (Input::checkButton(Keys::R_SHOULDER, States::HELD) || Input::getStrengthRnoMouse() > 0.01f)
				{
					ItemWeapon* temp = dynamic_cast<ItemWeapon*>(this->vehicleSlots->getSlot(Slots::MOUNTED));
					if (this->timeSinceLastShot >= temp->getWeapon().fireRate)
					{
						this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);

						for (int i = 0; i < Vehicle::bulletCount; ++i)
						{
							if (bullets[i].getWeaponType() == WeaponType::None)
							{
								auto playerVelocity = this->vehicle->getRigidBody()->getLinearVelocity();

								this->bullets[i].shoot(weapon,
									this->vehicleBody1->getPosition() + Vector3(curDir.x, 0, curDir.y),
									Vector3(curDir.x, 0.0, curDir.y),
									Vector3(playerVelocity.getX(), playerVelocity.getY(), playerVelocity.getZ()) * 0.5f);
								if (soundTimer > 4.0f) {
									int randomSound = rand() % 6 + 1;
									int rand2 = rand() % 2;
									std::wstring soundEffect = L"data/sound/MachineGunSound" + to_wstring(randomSound) + L".wav";
									if (rand2 < 1) {
										soundEffect = L"data/sound/MachineGunSound1.wav";
									}
									Sound::PlaySoundEffect(soundEffect);
									soundTimer = 0;
								}
								break;
							}
						}
					}
				}

				if (Input::checkButton(Keys::L_SHOULDER, States::HELD))
				{
					if (flameBool == true) {
						int randomSound = rand() % 2 + 1;
						std::wstring soundEffect = L"data/sound/FlameLoop" + to_wstring(randomSound) + L".wav";
						Sound::PlayLoopingSound(soundEffect);
						Sound::PlaySoundEffect(L"data/sound/FlameStart.wav");
						flameBool = false;
					}
					if (this->timeSinceLastShot2 >= this->weapon2.fireRate)
					{
						this->timeSinceLastShot2 = fmod(this->timeSinceLastShot2, this->weapon2.fireRate);

						for (int i = 0; i < Vehicle::bulletCount; ++i)
						{
							if (bullets[i].getWeaponType() == WeaponType::None)
							{
								auto playerVelocity = this->vehicle->getRigidBody()->getLinearVelocity();

								Vector3 tempDir = Vector3(cos(this->vehicleBody1->getRotation().y - 3.14 / 2), 0, -sin(this->vehicleBody1->getRotation().y - 3.14 / 2));
								this->bullets[i].shoot(weapon2,
									this->vehicleBody1->getPosition() + Vector3(tempDir * 1.5f) + Vector3(0.0f, -0.5f, 0.0f),
									tempDir,
									Vector3(playerVelocity.getX(), playerVelocity.getY(), playerVelocity.getZ()));
								break;
							}
						}
					}
				}
				else {
					flameBool = true;
					Sound::StopLoopingSound(L"data/sound/FlameLoop1.wav", true);
					Sound::StopLoopingSound(L"data/sound/FlameLoop2.wav", true);
				}
			}
		}

		if (dynamic_cast<PlayingGameState*>(Game::getCurrentState()) != nullptr)
		{
			for (int i = 0; i < Vehicle::bulletCount; i++)
			{
				bullets[i].update(deltaTime);
			}
			this->spotLight->setPos(this->vehicleSlots->getSlot(Slots::MOUNTED)->getObject()->getPosition() - Vector3(curDir.x, -1, curDir.y));
			this->spotLight->setDirection(Vector3(curDir.x, 0, curDir.y));
		}
	}
}

void Vehicle::setVehicleSlots(VehicleSlots* slots)
{
	this->vehicleSlots = slots;
}

float Vehicle::getAcceleratorX()
{
	return accelerator.x;
}

void Vehicle::setSpotLight(SpotLight* theLight)
{
	this->spotLight = theLight;
}

void Vehicle::setDrivingMode(int i)
{
	this->drivingMode = i;
}

bool Vehicle::getDrivingMode()
{
	return this->drivingMode;
}

Vector3 Vehicle::getVelocity()
{
	return Vector3(velocity.x,0,velocity.y);
}

float Vehicle::getVelocitySpeed()
{
	return velocitySpeed;
}

float Vehicle::getRotator()
{
	return this->mRotation /** (180 / DirectX::XM_PI)*/;
}

const int& Vehicle::getHealthRef() const
{
	return this->health;
}

int Vehicle::getHealth() const
{
	return this->health;
}

int Vehicle::getMaxHealth() const
{
	return this->updatedStats.maxHealth;
}

void Vehicle::setHealth(int health)
{
	this->health = std::clamp(health, 0, this->updatedStats.maxHealth);
}

void Vehicle::setMaxHealth(int maxHealth)
{
	this->updatedStats.maxHealth = max(maxHealth, 1);
}

void Vehicle::resetHealth()
{
	this->health = this->updatedStats.maxHealth;
}

void Vehicle::changeHealth(int amount)
{
	if (!this->immortal)
	{
		if (amount < 0) {
			dmg = true;
		}
		vehicleBody1->setColor(Vector4(max(vehicleBody1->getColor().x + -amount * 0.1f, 0), vehicleBody1->getColor().y, vehicleBody1->getColor().z, 1));
		this->health = std::clamp(this->health + amount, 0, this->updatedStats.maxHealth);
		Game::getGraphics().addParticle2(this->vehicle->getPosition(), Vector3(0, 0, 0), 2, 1);
		if (this->deadImpulse == false && this->health <= 0)
		{
			this->deadImpulse = true;
			vehicle->getRigidBody()->applyImpulse(btVector3(this->vehicle->getRigidBody()->getLinearVelocity().getX() * 6.3f, 0, this->vehicle->getRigidBody()->getLinearVelocity().getY() * 6.3f), btVector3(0, 0, 0));
			//vehicle->getRigidBody()->setAngularVelocity(btVector3(0,2000.0f,0));
			Game::getGraphics().addParticle(this->vehicle->getPosition() + Vector3(0, 2, 0), Vector3(0, 0, 0), 200, 10);
			dynamic_cast<PlayingGameState*>(Game::getCurrentState())->changeTime(-30);
		}
	}
}

bool Vehicle::isDead() const
{
	return this->health <= 0;
}

float Vehicle::getTotRespawnTime() const
{
	return this->totRespawnTime;
}

float Vehicle::getRespawnTimer() const
{
	return respawnTimer;
}

Vector3 Vehicle::getCameraDistance(float deltaTime)
{
	float vehicleDistance = 0;
	Vector3 vehicleCameraDistance = Vector3(this->vehicle->getRigidBody()->getLinearVelocity().getX(), this->vehicle->getRigidBody()->getLinearVelocity().getY(), this->vehicle->getRigidBody()->getLinearVelocity().getZ());
	vehicleDistance = max(abs(vehicleCameraDistance.x), abs(vehicleCameraDistance.z)) *0.7f;

	if (vehicleDistance < 10.0f) {
		vehicleDistance = 0.0f;
	}
	vehicleDistance = min(vehicleDistance, 20.0f);

	cameraDistance = (vehicleDistance - cameraDistance) * deltaTime*1.2f + cameraDistance;
	cameraDistanceX = ((this->vehicle->getRigidBody()->getLinearVelocity().getX() * 0.15f + Input::getDirectionR().x * Input::getStrengthR()*3) - cameraDistanceX) * deltaTime * 2.2f + cameraDistanceX;
	cameraDistanceZ = ((this->vehicle->getRigidBody()->getLinearVelocity().getZ() * 0.15f + Input::getDirectionR().y * Input::getStrengthR()*3) - cameraDistanceZ) * deltaTime * 2.2f + cameraDistanceZ;

	return Vector3(cameraDistanceX,cameraDistance,cameraDistanceZ);
}

void Vehicle::setAccelForce(Vector3 accelForce, float deltaTime)
{

	//AccelForce
	if ((max(abs(accelForce.x), abs(accelForce.z)) > 5.0f)) {
		int randomSound = rand() % 3 + 1;
		std::wstring soundEffect = L"data/sound/CarImpact" + to_wstring(randomSound) + L".wav";
		int randomSound2 = rand() % 3 + 1;
		std::wstring soundEffect2 = L"data/sound/MetalImpactPitched" + to_wstring(randomSound) + L".wav";
		if (max(abs(accelForce.x), abs(accelForce.z)) > 25.0f) {
			Game::getGraphics().addParticle2(this->vehicle->getPosition(), Vector3(0, 0, 0), 2, 1);
			changeHealth(-20.0f);
			Sound::PlaySoundEffect(L"data/sound/CarCrash.wav");
			Sound::PlaySoundEffect(soundEffect2);
		}
		else if (max(abs(accelForce.x), abs(accelForce.z)) > 15.0f) {
			Game::getGraphics().addParticle2(this->vehicle->getPosition(), Vector3(0, 0, 0), 2, 1);
			changeHealth(-10.0f);
			Sound::PlaySoundEffect(soundEffect);
			Sound::PlaySoundEffect(soundEffect2);
		}
		else {
			Sound::PlaySoundEffect(L"data/sound/CarImpactSoft.wav");
			Sound::PlaySoundEffect(soundEffect2);
		}
	}
	/*else {
		vehicleBody1->setColor(Vector4(vehicleBody1->getColor().x/(1 + 20.0f*deltaTime), vehicleBody1->getColor().y / (1 + 20.0f * deltaTime), vehicleBody1->getColor().z / (1 + 20.0f * deltaTime), 1));
	}*/
}

void Vehicle::setWheelRotation()
{
	wheel1->setPosition(vehicle->getPosition() + Vector3(0,0.25f,0));
	wheel2->setPosition(vehicle->getPosition() + Vector3(0, 0.25f, 0));
	wheel3->setPosition(vehicle->getPosition() + Vector3(0, 0.25f, 0));
	wheel4->setPosition(vehicle->getPosition() + Vector3(0, 0.25f, 0));

	wheel1->setRotation(Vector3(0,vehicleBody1->getRotation().y,0));
	wheel2->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
	wheel3->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
	wheel4->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
}

float Vehicle::getPitch(DirectX::XMVECTOR Quaternion)
{
	return atan2(2 * (Quaternion.m128_f32[1] * Quaternion.m128_f32[2] + Quaternion.m128_f32[3] * Quaternion.m128_f32[0]), Quaternion.m128_f32[3] * Quaternion.m128_f32[3] - Quaternion.m128_f32[0] * Quaternion.m128_f32[0] - Quaternion.m128_f32[1] * Quaternion.m128_f32[1] + Quaternion.m128_f32[2] * Quaternion.m128_f32[2]);
}

float Vehicle::getYaw(DirectX::XMVECTOR Quaternion)
{
	return asin(-2 * (Quaternion.m128_f32[0] * Quaternion.m128_f32[2] - Quaternion.m128_f32[3] * Quaternion.m128_f32[1]));
}

float Vehicle::getRoll(DirectX::XMVECTOR Quaternion)
{
	return atan2(2 * (Quaternion.m128_f32[0] * Quaternion.m128_f32[1] + Quaternion.m128_f32[3] * Quaternion.m128_f32[2]), Quaternion.m128_f32[3] * Quaternion.m128_f32[3] + Quaternion.m128_f32[0] * Quaternion.m128_f32[0] - Quaternion.m128_f32[1] * Quaternion.m128_f32[1] - Quaternion.m128_f32[2] * Quaternion.m128_f32[2]);
}

float Vehicle::getHeading(Quaternion qt)
{
	float heading;
	float attitude;
	float bank;

	float test = qt.x * qt.y + qt.z * qt.w;
	if (test > 0.499) { // singularity at north pole
		heading = 2 * atan2(qt.x, qt.w);
		attitude = XM_PI / 2;
		bank = 0;
	}
	else if (test < -0.499) { // singularity at south pole
		heading = -2 * atan2(qt.x, qt.w);
		attitude = -XM_PI / 2;
		bank = 0;
	}
	else {
		float sqx = qt.x * qt.x;
		float sqy = qt.y * qt.y;
		float sqz = qt.z * qt.z;
		heading = atan2(2 * qt.y * qt.w - 2 * qt.x * qt.z, 1 - 2 * sqy - 2 * sqz);
		attitude = asin(2 * test);
		bank = atan2(2 * qt.x * qt.w - 2 * qt.y * qt.z, 1 - 2 * sqx - 2 * sqz);
	}
	return heading;
}

Bullet* Vehicle::getBulletArray(size_t& count)
{
	count = bulletCount;
	return bullets;
}

void Vehicle::powerUp(PowerUpType type)
{
	if (type == PowerUpType::Time)
	{
		dynamic_cast<PlayingGameState*>(Game::getCurrentState())->changeTime(30);
	}
	else if (type == PowerUpType::Health)
	{
		this->changeHealth(100);
	}
	else if (type == PowerUpType::Speed)
	{
		this->powerUpTimers[(int)PowerUpType::Speed] += 30.0;

	}
}
