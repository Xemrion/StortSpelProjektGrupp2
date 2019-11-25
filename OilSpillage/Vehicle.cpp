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
	this->totalRespawnTime = 5.0f;
	this->immortalTimer = 0.0f;
	this->immortal = false;

	targetRotation = 0.0f;
	drivingMode = 1;
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
	this->accelForce = Vector3(0, 0, 0);
	this->reverseTimer = 0;
	this->reverseTimer2 = 0;

	this->curDir = Vector2(0.0f, 1.0f);//UP

	this->defaultStats = VehicleStats::fastCar;
	this->updatedStats = this->defaultStats;
	this->health = this->updatedStats.maxHealth;

	this->vehicleSlots = new VehicleSlots;
	
}

Vehicle::~Vehicle()
{
	delete vehicle;
	delete vehicleBody1;

	delete wheel1;
	delete wheel2;
	delete wheel3;
	delete wheel4;
	//delete spring1;
	//delete pointJoint;
	delete this->vehicleSlots;
}

void Vehicle::init(Physics* physics)
{
	this->physics = physics;
	

	this->vehicle = new GameObject;
	Game::getGraphics().loadShape(Shapes::SHAPE_CUBE);
	vehicle->mesh = Game::getGraphics().getMeshPointer("Cube");
	//Game::getGraphics().addToDraw(vehicle);
	vehicle->setPosition(Vector3(0.0f, 10.0f, 0.0f));
	vehicle->setScale(Vector3(0.5f, 0.14f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp");
	vehicle->setTexture(Game::getGraphics().getTexturePointer("CarTemp"));
	Game::getGraphics().loadModel("Entities/Turret");
	

	//this->vehicleBody1 = new GameObject;
	//vehicleBody1->mesh = Game::getGraphics().getMeshPointer("Entities/Player1");
	//vehicleBody1->setSpotShadow(false);
	//Game::getGraphics().addToDraw(vehicleBody1);
	//vehicleBody1->setScale(Vector3(0.005f, 0.005f, 0.005f));
	////vehicleBody1->setPosition(Vector3(0.0f, 0.65f, 0.0f));
	////vehicleBody1->setScale(Vector3(0.5f, 0.22f, 0.9f));

	//vehicleBody1->setTexture(Game::getGraphics().getMaterial("Entities/Player").diffuse);
	
	this->vehicleBody1 = new GameObject;
	Game::getGraphics().loadModel("Entities/Player_Car_Parts/Chassi1");
	vehicleBody1->mesh = Game::getGraphics().getMeshPointer("Entities/Player_Car_Parts/Chassi1");
	vehicleBody1->setSpotShadow(false);
	Game::getGraphics().addToDraw(vehicleBody1);
	vehicleBody1->setScale(Vector3(0.35f, 0.35f, 0.35f));	
	//vehicleBody1->setPosition(Vector3(0.0f, 0.65f, 0.0f));
	//vehicleBody1->setScale(Vector3(0.5f, 0.22f, 0.9f));
	vehicleBody1->setMaterial(Game::getGraphics().getMaterial("Entities/Player_Car_Parts/Chassi1"));

	Game::getGraphics().loadModel("Entities/Player_Car_Parts/Wheel1");
	this->wheel1 = new GameObject;
	this->wheel2 = new GameObject;
	this->wheel3 = new GameObject;
	this->wheel4 = new GameObject;
	wheel1->mesh = Game::getGraphics().getMeshPointer("Entities/Player_Car_Parts/Wheel1");
	wheel2->mesh = Game::getGraphics().getMeshPointer("Entities/Player_Car_Parts/Wheel1");
	wheel3->mesh = Game::getGraphics().getMeshPointer("Entities/Player_Car_Parts/Wheel1");
	wheel4->mesh = Game::getGraphics().getMeshPointer("Entities/Player_Car_Parts/Wheel1");
	Game::getGraphics().addToDraw(wheel1);
	Game::getGraphics().addToDraw(wheel2);
	Game::getGraphics().addToDraw(wheel3);
	Game::getGraphics().addToDraw(wheel4);
	wheel1->setPosition(Vector3(1.0f, 1.0f, 0.0f));
	wheel2->setPosition(Vector3(-1.0f, 1.0f, 0.0f));
	wheel3->setPosition(Vector3(0.0f, 1.0f, 1.0f));
	wheel4->setPosition(Vector3(0.0f, 1.0f, -1.0f));
	wheel1->setScale(Vector3(0.38f, 0.38f, 0.38f));
	wheel2->setScale(Vector3(0.38f, 0.38f, 0.38f));
	wheel3->setScale(Vector3(0.38f, 0.38f, 0.38f));
	wheel4->setScale(Vector3(0.38f, 0.38f, 0.38f));
	wheel1->setMaterial(Game::getGraphics().getMaterial("Entities/Player_Car_Parts/Wheel1"));
	wheel2->setMaterial(Game::getGraphics().getMaterial("Entities/Player_Car_Parts/Wheel1"));
	wheel3->setMaterial(Game::getGraphics().getMaterial("Entities/Player_Car_Parts/Wheel1"));
	wheel4->setMaterial(Game::getGraphics().getMaterial("Entities/Player_Car_Parts/Wheel1"));


	//btRigidBody* tempo = physics->addBox(btVector3(vehicle->getPosition().x, vehicle->getPosition().y, vehicle->getPosition().z), btVector3(vehicle->getScale().x, vehicle->getScale().y, vehicle->getScale().z), 10.0f);
	btRigidBody* tempo = physics->addCapsule(vehicle->getScale().x,btVector3(vehicle->getPosition().x, vehicle->getPosition().y, vehicle->getPosition().z), vehicle->getScale().z, 10.0f);
	this->getVehicle()->setRigidBody(tempo, physics);
	this->getVehicle()->getRigidBody()->activate();
	this->getVehicle()->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	this->getVehicle()->getRigidBody()->setFriction(0);
	this->getVehicle()->getRigidBody()->setLinearFactor(btVector3(1, 0, 1));
	this->getVehicle()->getRigidBody()->setLinearVelocity(btVector3(5, 0, 1));

	tempo = physics->addBox(
		btVector3(vehicle->getPosition().x, vehicle->getPosition().y + 0.65f, vehicle->getPosition().z),
		-btVector3(this->vehicleBody1->getAABB().minPos.x - this->vehicleBody1->getAABB().maxPos.x, (this->vehicleBody1->getAABB().minPos.y - this->vehicleBody1->getAABB().maxPos.y) * 0.2f, this->vehicleBody1->getAABB().minPos.z - this->vehicleBody1->getAABB().maxPos.z) * 0.5f,
		1.0f,
		this);
	vehicleBody1->setRigidBody(tempo, physics);
	vehicleBody1->getRigidBody()->activate();
	vehicleBody1->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	vehicleBody1->getRigidBody()->setCollisionFlags(vehicleBody1->getRigidBody()->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
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
}

void Vehicle::updatePlayer(float deltaTime)
{
	this->update(deltaTime, Input::getStrengthL(),Input::checkButton(Keys::R_TRIGGER,States::HELD) || Input::isKeyDown_DEBUG(Keyboard::W), Input::checkButton(Keys::L_TRIGGER,States::HELD) || Input::isKeyDown_DEBUG(Keyboard::S),Input::getDirectionL());
}

void Vehicle::update(float deltaTime, float throttleInputStrength, bool throttleInputTrigger, bool reverseInputTrigger, Vector2 directionInput)
{
	PlayingGameState* playing = dynamic_cast<PlayingGameState*>(Game::getCurrentState());

	if (playing != nullptr)
	{
		if (this->deadImpulse == true && this->health <= 0)
		{
			this->respawnTimer += deltaTime;
			if (this->respawnTimer > 5)
			{
				this->resetHealth();
				this->immortal = true;
				this->immortalTimer = 5.0f;
				this->vehicle->setPosition(playing->getRespawnPosition());
				this->vehicleBody1->setPosition(playing->getRespawnPosition() + Vector3(0.0f, 0.65f, 0.0f));

				this->deadImpulse = false;
				this->respawnTimer = 0.0f;
			}
		}
		if (playing->getTime() == 0)
		{
			this->respawnTimer = 0.0f;
		}

		updatePowerUpEffects(deltaTime);
	}


	tempTargetRotation = targetRotation;

	//Quaternion Rotation to Euler
	btScalar m;
	btScalar n;
	Quaternion qt = Vector4(this->vehicle->getRigidBody()->getWorldTransform().getRotation().getX(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getY(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getZ(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getW());

	qt = this->vehicle->getRotationQuaternion();

	float heading = getHeading(qt);

	this->vehicle->getRigidBody()->getWorldTransform().getRotation().getEulerZYX(n, mRotation, m);

	//Current rotation of vehicle
	float vehicleRotation = fmod((/*(getYaw(qt)*/heading * 180 / DirectX::XM_PI), 360.0f);
	if (vehicleRotation < 0)
		vehicleRotation += 360;
	mRotation = vehicleRotation;

	//Rotate Chassis toward Body
	vehicleBody1->getRigidBody()->applyImpulse(btVector3(0, 80 * deltaTime, 0), btVector3(0, 10, 0));
	vehicleBody1->getRigidBody()->setDamping(0, 0.99f);
	qt = Vector4(this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getX(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getY(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getZ(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getW());
	float bodyHeading = getHeading(qt);

	bodyHeading = fmod((bodyHeading * 180 / DirectX::XM_PI), 360.0f);
	heading = fmod((heading * 180 / DirectX::XM_PI), 360.0f);
	float difference = (180 - abs(abs(bodyHeading - heading) - 180)) * 0.45f; ;
	if (bodyHeading < heading) {
		if (abs(bodyHeading - heading) < 180) {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}
		else {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), -difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}
	}
	else {
		if (abs(bodyHeading - heading) < 180) {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), -difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}
		else {
			vehicleBody1->getRigidBody()->setAngularVelocity(btVector3(vehicleBody1->getRigidBody()->getAngularVelocity().getX(), difference, vehicleBody1->getRigidBody()->getAngularVelocity().getZ()));
		}
	}

	//Direction of Vehicle in dx & dy
	float dx = sin((DirectX::XM_PI / 180) * vehicleRotation);
	float dy = -cos((DirectX::XM_PI / 180) * vehicleRotation);


	targetRotation = (atan2(-directionInput.x, -directionInput.y) * 180 / DirectX::XM_PI) + 180;
	velocitySpeed = (this->vehicle->getRigidBody()->getLinearVelocity().getX() * (dx)) + (-this->vehicle->getRigidBody()->getLinearVelocity().getZ() * (dy));

	Vector3 steering = Vector3(vehicle->getRigidBody()->getAngularVelocity().getX(),
		throttleInputStrength * updatedStats.handlingRate * /*deltaTime*/0.035f * 80 * min(velocitySpeed * 0.15f, 1),
		vehicle->getRigidBody()->getAngularVelocity().getZ());

	Vector3 steering2 = Vector3(vehicle->getRigidBody()->getAngularVelocity().getX(),
		throttleInputStrength * updatedStats.handlingRate * /*deltaTime*/0.035f * 80 * max(velocitySpeed * 0.15f, -1),
		vehicle->getRigidBody()->getAngularVelocity().getZ());

	//Driving mode: Throttle and turning, realistic
	if (drivingMode == 0) {
		if ((throttleInputTrigger) && this->health > 0) {
			if (velocitySpeed < (40 * updatedStats.maxSpeed)) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(dx * deltaTime * 160.0f * updatedStats.accelerationRate, 0, -(dy * deltaTime * 160.0f * updatedStats.accelerationRate)), btVector3(0, 0, 0));
			}
		}
		if (directionInput.x > 0 && this->health > 0) {
			if (velocitySpeed > 0.0f) {
				if (rotationDirection == true) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 10 * min(velocitySpeed * 0.15f, 1) * throttleInputStrength;
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * throttleInputStrength) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * throttleInputStrength;
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, steering.y * min(rotateAcceleration, 1), steering.z));
				rotationDirection = false;
			}
			else {
				if (rotationDirection == false) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 10 * min(velocitySpeed * 0.15f, 1) * throttleInputStrength;
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * throttleInputStrength) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * throttleInputStrength;
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, steering2.y/* * min(rotateAcceleration, 1)*/, steering.z));
				rotationDirection = true;
			}
		}
		else if (directionInput.x < 0 && this->health>0) {
			if (velocitySpeed > 0.0f) {
				if (rotationDirection == true) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 10 * min(velocitySpeed * 0.15f, 1) * throttleInputStrength;
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * throttleInputStrength) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * throttleInputStrength;
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, -steering.y * min(rotateAcceleration, 1), steering.z));
				rotationDirection = false;
			}
			else {
				if (rotationDirection == false) {
					rotateAcceleration = 0;
				}
				rotateAcceleration += deltaTime * 10 * min(velocitySpeed * 0.15f, 1) * throttleInputStrength;
				if (rotateAcceleration > 3.04f * min(velocitySpeed * 0.15f, 1) * throttleInputStrength) {
					rotateAcceleration = 3.04f * min(velocitySpeed * 0.15f, 1) * throttleInputStrength;
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
		if (reverseInputTrigger && velocitySpeed > (-40 * updatedStats.maxSpeed) && this->health > 0) {
			this->vehicle->getRigidBody()->applyImpulse(btVector3(-(dx * deltaTime * 160.0f * 0.7f * updatedStats.accelerationRate), 0, (dy * deltaTime * 160.0f * 0.7f * updatedStats.accelerationRate)), btVector3(0, 0, 0));

			if (velocitySpeed > 0.0f) {
				vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (1.8f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (1.8f * deltaTime))));
			}
		}
		if (!throttleInputTrigger && !reverseInputTrigger) {
			vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (0.6f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (0.6f * deltaTime))));
		}
	}
	//Driving Mode: Turn towards direction, semi-realistic
	else if (drivingMode == 1) {

		Vector3 steering3 = Vector3(vehicle->getRigidBody()->getAngularVelocity().getX(),
			/*deltaTime*/0.035f * updatedStats.handlingRate * 80 * min(velocitySpeed * 0.15f, 1),
			vehicle->getRigidBody()->getAngularVelocity().getZ());

		if (throttleInputStrength > 0 && this->health > 0) {
			if (velocitySpeed < 0.5f) {
				reverseTimer += 10.0f * deltaTime;
			}
			else {
				reverseTimer = 0;
				reverseTimer2 = 0;
			}
			if ((reverseTimer > 10.0f) && !(reverseTimer2 > 10.0f)) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(-dx * deltaTime * 160.0f * updatedStats.accelerationRate, 0, (dy * deltaTime * 160.0f * updatedStats.accelerationRate)) * throttleInputStrength, btVector3(0, 0, 0));
				reverseTimer2 += 10.0f * deltaTime;
				if (reverseTimer2 > 10.0f) {
					reverseTimer = 0;
					reverseTimer2 = 0;
				}
			}
			else {
				reverseTimer2 = 0;
				if (velocitySpeed < (40 * updatedStats.maxSpeed)) {
					this->vehicle->getRigidBody()->applyImpulse(btVector3(dx * deltaTime * 160.0f * updatedStats.accelerationRate, 0, -(dy * deltaTime * 160.0f * updatedStats.accelerationRate)) * throttleInputStrength, btVector3(0, 0, 0));
				}
			}

			if (strength < throttleInputStrength) {
				strength = throttleInputStrength;
			}
			else {
				strength -= 0.02f * deltaTime;
			}

			float difference2 = min((180 - abs(abs(vehicleRotation - targetRotation) - 180)) * 0.05f, 1.0f);

			if (vehicleRotation < targetRotation) {
				if (abs(vehicleRotation - targetRotation) < 180) {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, steering3.y * difference2, 0));
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
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, steering3.y * difference2, 0));
				}
			}
		}
		else if (health > 0) {
			vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (1.8f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (1.8f * deltaTime))));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 40;
			//vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime*2 * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
			vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180) * deltaTime * 120 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
		}
		else {
			vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (0.6f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (0.6f * deltaTime))));
			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 40;

			vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, (vehicle->getRigidBody()->getAngularVelocity().getY() / (1 + (0.8f * deltaTime)))/** min(abs(velocitySpeed), 1)*/, steering.z));
		}

		rotateAcceleration /= 1.0f + 6 * deltaTime;
	}


	//Drifting
	float hypoC = sqrt(pow(dx, 2) + (pow(dy, 2)));
	this->driftForce = this->vehicle->getRigidBody()->getLinearVelocity().getX() * (dy / hypoC) + -this->vehicle->getRigidBody()->getLinearVelocity().getZ() * -(dx / hypoC);
	Vector2 driftResistance = Vector2(-((dy / hypoC) * 4000 * deltaTime) * updatedStats.handlingRate, -(-((dx / hypoC) * 4000 * deltaTime)) * updatedStats.handlingRate);
	if (abs(driftForce) < 250) {
		driftResistance = driftResistance * (abs(driftForce) * 0.005f);
	}
	if (!(health > 0)) {
		//driftResistance /= 2;
	}
	if (Input::isKeyDown_DEBUG(DirectX::Keyboard::LeftShift)) {
		driftResistance /= 2;
	}
	if (drivingMode != 2) {
		if (Input::getStrengthL() > 0) {
			if (driftForce < -0) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(-driftResistance.x, 0, 0), btVector3(0, 0, 0));
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


	for (int i = 0; i < Slots::SIZEOF; i++)
	{
		Item* temp = this->vehicleSlots->getSlot(Slots(i));
		if (temp != nullptr)
		{
			if (temp->getType() == ItemType::WEAPON)
			{
				ItemWeapon* tempWeapon = dynamic_cast<ItemWeapon*>(temp);
				if (tempWeapon != nullptr)
				{
					tempWeapon->getWeapon().updateWeapon(deltaTime);//for sound and timeSinceLastShot
				}
			}
		}
	}

	add += 0.4f * deltaTime;

	/*if (dmg == true) {
		vehicleBody1->setColor(Vector4(vehicleBody1->getColor().x / (1 + 15.0f * deltaTime), vehicleBody1->getColor().y, vehicleBody1->getColor().z, 1));
		if (vehicleBody1->getColor().x <= 0.01f) {
			dmg = false;
		}
	}*/
	vehicleBody1->setColor(Vector4(vehicleBody1->getColor().x / (1 + 5.0f * deltaTime), vehicleBody1->getColor().y / (1 + 5.0f * deltaTime), vehicleBody1->getColor().z / (1 + 5.0f * deltaTime), 1));



	//Engine sound
	engineGears += (deltaTime * 0.15f + randomGears*0.5f *deltaTime * 0.05f)*Input::getStrengthL();
	if (engineGears > 1) {
		randomGears = rand() % 10 + 1;
		engineGears = ((rand() % 11)-5)*0.1f;
		if (abs(driftForce) < 15.0f) {
			int randomSound = rand() % 4 + 1;
			std::string soundEffect = "./data/sound/CarExhaust" + std::to_string(randomSound) + ".mp3";
			Sound::play(soundEffect,0.6f);
		}
	}
	enginePitch = enginePitch + deltaTime*2 * ((Input::getStrengthL()) - enginePitch);
	time += deltaTime;
	if (abs(driftForce) < 15.0f) {
		Sound::changeLoopingPitch(soundHandle, 1.0f + enginePitch + (engineGears * Input::getStrengthL() * min(abs(velocitySpeed * 0.045f), 1)));
		Sound::changeLoopingVolume(soundHandle, 0.7f);
		driftVolume -= 1 * deltaTime;
		Sound::changeLoopingVolume(driftHandle,driftVolume);
		driftBool = false;
	}
	else {
		driftVolume = 0.2f;
		int randomSound = rand() % 2 + 1;
		std::string soundEffect = "./data/sound/Drift" + std::to_string(randomSound) + ".mp3";
		if (driftBool == false) {
			Sound::stopLooping(driftHandle);
			driftHandle = Sound::playLooping(soundEffect, 0.2f);
			driftBool = true;
		}
		Sound::changeLoopingPitch(soundHandle, 3);
		Sound::changeLoopingVolume(soundHandle, min(20 * sin((time - 1) / 0.0095f) + 0, 0.7f));
	}

	if (this->immortal)
	{
		this->immortalTimer -= deltaTime;
		float sine = sin((this->immortalTimer - 0) / 0.3f) - 0.5f;
		float color = max(sine, 0.0f);
		this->vehicleBody1->setColor(Vector4(color, color, color, color));
		if (this->immortalTimer <= 0)
		{
			this->immortalTimer = 0.0f;
			this->immortal = false;
		}
	}

	/*Trail / Sladdspår*/
	this->trailTimer += deltaTime;

	Vector3 frontTempDir = Vector3(cos(this->vehicleBody1->getRotation().y - 3.14 / 2), 0, -sin(this->vehicleBody1->getRotation().y - 3.14 / 2));
	Vector3 rightDir = frontTempDir.Cross(Vector3(0.0f, 1.0f, 0.0f));
	rightDir.Normalize();
	Vector3 initialDir = -Vector3(this->vehicle->getRigidBody()->getLinearVelocity());
	initialDir.Normalize();
	if (this->trailTimer > 0.01f && abs(this->driftForce) > 5.0f)
	{
		Game::getGraphics().addTrail(Vector3(0.0f, -0.6f, 0.0f) - frontTempDir * 0.01f + this->vehicleBody1->getPosition() + rightDir * 0.5f, Vector4(initialDir.x, initialDir.y, initialDir.z, 0.1f * abs(this->driftForce)), 1, 60.0f);
		Game::getGraphics().addTrail(Vector3(0.0f, -0.6f, 0.0f) - frontTempDir * 0.01f + this->vehicleBody1->getPosition() - rightDir * 0.5f, Vector4(initialDir.x, initialDir.y, initialDir.z, 0.1f * abs(this->driftForce)), 1, 60.0f);

		Game::getGraphics().addTrail(Vector3(0.0f, -0.6f, 0.0f) + frontTempDir * 1.0f + this->vehicleBody1->getPosition() + rightDir * 0.5f, Vector4(initialDir.x, initialDir.y, initialDir.z, 0.1f * abs(this->driftForce)), 1, 60.0f);
		Game::getGraphics().addTrail(Vector3(0.0f, -0.6f, 0.0f) + frontTempDir * 1.0f + this->vehicleBody1->getPosition() - rightDir * 0.5f, Vector4(initialDir.x, initialDir.y, initialDir.z, 0.1f * abs(this->driftForce)), 1, 60.0f);
		this->trailTimer = 0;
	}
}

void Vehicle::updateWeapon(float deltaTime)
{
	if (this->vehicleSlots != nullptr)
	{
		float yaw = this->vehicleBody1->getRotation().y;
		float pitch = this->vehicleBody1->getRotation().z;

		Vector3 frontTempDir = Vector3(cos(this->vehicleBody1->getRotation().y - 3.14 / 2), 0, -sin(this->vehicleBody1->getRotation().y - 3.14 / 2));
		Vector3 tempDirYawPtich = Vector3(cos(yaw) * cos(pitch), sin(yaw) * cos(pitch), sin(pitch));
		tempDirYawPtich.Normalize();
		float angleWP = tempDirYawPtich.Dot(Vector3(0, 0, 1));
		/*MOUNTED*/
		if (this->vehicleSlots->getSlot(Slots::MOUNTED) != nullptr)
		{
			if (this->vehicleSlots->getSlot(Slots::MOUNTED)->getObject() != nullptr)
			{
				if (dynamic_cast<PlayingGameState*>(Game::getCurrentState()) == nullptr)
				{
					//in upgradingstate 
					this->vehicleSlots->getSlot(Slots::MOUNTED)->getObject()->setRotation(Vector3(0, this->vehicleBody1->getRotation().y, 0));// acos(angleWP)));
				}
				this->vehicleSlots->getSlot(Slots::MOUNTED)->getObject()->setPosition(vehicleBody1->getPosition()+Vector3(0,0.5f,0));
				this->vehicleSlots->getSlot(Slots::MOUNTED)->getObject()->setScale(Vector3(0.2f, 0.2f, 0.2f));
			}
		}

		/*END*/

		/*FRONT*/
		if (this->vehicleSlots->getSlot(Slots::FRONT) != nullptr)
		{
			if (this->vehicleSlots->getSlot(Slots::FRONT)->getObject() != nullptr)
			{
				this->vehicleSlots->getSlot(Slots::FRONT)->getObject()->setRotation(Vector3(0, this->vehicleBody1->getRotation().y, acos(angleWP) - 3.14 / 2));
				this->vehicleSlots->getSlot(Slots::FRONT)->getObject()->setPosition(this->vehicleBody1->getPosition() + 0.65f * frontTempDir - Vector3(0.0f, 0.25f, 0.0f));
				this->vehicleSlots->getSlot(Slots::FRONT)->getObject()->setScale(Vector3(0.15f, 0.15f, 0.15f));
			}
		}
		/*END*/

		/*BACK*/
		if (this->vehicleSlots->getSlot(Slots::BACK) != nullptr)
		{
			if (this->vehicleSlots->getSlot(Slots::BACK)->getObject() != nullptr)
			{
				this->vehicleSlots->getSlot(Slots::BACK)->getObject()->setPosition(this->vehicleBody1->getPosition() - 0.65f * frontTempDir - Vector3(0.0f, 0.25f, 0.0f));
				this->vehicleSlots->getSlot(Slots::BACK)->getObject()->setRotation(Vector3(0, this->vehicleBody1->getRotation().y + 3.14f, acos(angleWP) - 3.14 / 2));
				this->vehicleSlots->getSlot(Slots::BACK)->getObject()->setScale(Vector3(0.15f, 0.15f, 0.15f));
			}
		}
		/*END*/

		/*RIGHT*/

		Vector3 upp(0, 1, 0);
		Vector3 right = upp.Cross(frontTempDir);
		if (this->vehicleSlots->getSlot(Slots::RIGHT) != nullptr)
		{
			if (this->vehicleSlots->getSlot(Slots::RIGHT)->getObject() != nullptr)
			{
				this->vehicleSlots->getSlot(Slots::RIGHT)->getObject()->setPosition(this->vehicleBody1->getPosition() - 0.35f * right - Vector3(0.0f, 0.25f, 0.0f));
				this->vehicleSlots->getSlot(Slots::RIGHT)->getObject()->setRotation(Vector3(0, this->vehicleBody1->getRotation().y - 3.14 / 2, acos(angleWP) - 3.14 / 2));
				this->vehicleSlots->getSlot(Slots::RIGHT)->getObject()->setScale(Vector3(0.15f,0.15f,0.15f));
			}
		}
		/*END*/

		/*LEFT*/
		if (this->vehicleSlots->getSlot(Slots::LEFT) != nullptr)
		{
			if (this->vehicleSlots->getSlot(Slots::LEFT)->getObject() != nullptr)
			{
				this->vehicleSlots->getSlot(Slots::LEFT)->getObject()->setPosition(this->vehicleBody1->getPosition() + 0.35f * right - Vector3(0.0f, 0.25f, 0.0f));
				this->vehicleSlots->getSlot(Slots::LEFT)->getObject()->setRotation(Vector3(0, this->vehicleBody1->getRotation().y + 3.14 / 2, acos(angleWP) - 3.14 / 2));
				this->vehicleSlots->getSlot(Slots::LEFT)->getObject()->setScale(Vector3(0.15f, 0.15f, 0.15f));
			}
		}
		/*END*/
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
			curDir = Vector2::Lerp(curDir, dir, deltaTime * 50);
			curDir.Normalize();
		}
		else
		{
			curDir = dir;
		}

		if (this->health > 0)
		{
			if (dynamic_cast<PlayingGameState*>(Game::getCurrentState()) != nullptr)
			{
				float newRot = atan2(curDir.x, curDir.y);
				this->gunRotation = newRot;

				if (this->vehicleSlots->getSlot(Slots::MOUNTED) != nullptr)
				{
					if (this->vehicleSlots->getSlot(Slots::MOUNTED)->getObject() != nullptr)
					{
						ItemWeapon* temp = dynamic_cast<ItemWeapon*>(this->vehicleSlots->getSlot(Slots::MOUNTED));
						this->vehicleSlots->getSlot(Slots::MOUNTED)->getObject()->setRotation(Vector3(0, newRot, 0));

						if (/*Input::checkButton(Keys::R_SHOULDER, States::HELD) ||*/ Input::getStrengthRnoMouse() > 0.01f || Input::checkButtonMouse(MouseKeys::LEFT,States::HELD))
						{
							if (temp->getWeapon().updateFireRate())
							{
						
								for (int i = 0; i < Vehicle::bulletCount; ++i)
								{
									if (bullets[i].getWeaponType() == WeaponType::None)
									{
										auto playerVelocity = this->vehicle->getRigidBody()->getLinearVelocity();
										this->bullets[i].shoot(temp->getWeapon(),
											this->vehicleBody1->getPosition() + Vector3(curDir.x, 0, curDir.y),
											Vector3(curDir.x, 0.0, curDir.y),
											Vector3(playerVelocity.getX(), playerVelocity.getY(), playerVelocity.getZ()) * 1.0f, deltaTime);

										WeaponHandler::weaponStartSound(temp->getWeapon());
										break;
									}
								}
							}

						}
						else
						{
							WeaponHandler::weaponEndSound(temp->getWeapon());
						}
					}
				}
				if (this->vehicleSlots->getSlot(Slots::FRONT) != nullptr)
				{
					if (this->vehicleSlots->getSlot(Slots::FRONT)->getObject() != nullptr)
					{
						ItemWeapon* temp = dynamic_cast<ItemWeapon*>(this->vehicleSlots->getSlot(Slots::FRONT));
						if (this->vehicleSlots->getSlot(Slots::FRONT)->getType() == ItemType::WEAPON && temp!=nullptr)
						{
							if (Input::checkButton(Keys::R_SHOULDER, States::HELD))
							{
								//weapon.startSound();
								WeaponHandler::weaponStartSound(temp->getWeapon());
								if (temp->getWeapon().updateFireRate())
								{
									for (int i = 0; i < Vehicle::bulletCount; ++i)
									{
										if (bullets[i].getWeaponType() == WeaponType::None)
										{
											auto playerVelocity = this->vehicle->getRigidBody()->getLinearVelocity();

											Vector3 tempDir = Vector3(cos(this->vehicleBody1->getRotation().y - 3.14 / 2), 0, -sin(this->vehicleBody1->getRotation().y - 3.14 / 2));
											this->bullets[i].shoot(temp->getWeapon(),
												this->vehicleBody1->getPosition() + Vector3(tempDir * 1.5f) + Vector3(0.0f, -0.5f, 0.0f),
												tempDir,
												Vector3(playerVelocity.getX(), playerVelocity.getY(), playerVelocity.getZ()), deltaTime);
											break;
										}
									}
								}
							}
							else
							{
								WeaponHandler::weaponEndSound(temp->getWeapon());
							}
						}
					}
				}
				if (this->vehicleSlots->getSlot(Slots::BACK) != nullptr)
				{
					if (this->vehicleSlots->getSlot(Slots::BACK)->getObject() != nullptr)
					{
						ItemWeapon* temp = dynamic_cast<ItemWeapon*>(this->vehicleSlots->getSlot(Slots::BACK));
						if (this->vehicleSlots->getSlot(Slots::BACK)->getType() == ItemType::WEAPON && temp != nullptr)
						{
							if (Input::checkButton(Keys::CONFIRM, States::HELD))
							{
								//weapon.startSound();
								WeaponHandler::weaponStartSound(temp->getWeapon());
								if (temp->getWeapon().updateFireRate())
								{
									for (int i = 0; i < Vehicle::bulletCount; ++i)
									{
										if (bullets[i].getWeaponType() == WeaponType::None)
										{
											auto playerVelocity = this->vehicle->getRigidBody()->getLinearVelocity();

											Vector3 tempDir = Vector3(cos(this->vehicleBody1->getRotation().y - 3.14 / 2), 0, -sin(this->vehicleBody1->getRotation().y - 3.14 / 2));
											this->bullets[i].shoot(temp->getWeapon(),
												this->vehicleBody1->getPosition() + Vector3(-tempDir * 1.5f) + Vector3(0.0f, -0.5f, 0.0f),
												-tempDir,
												Vector3(playerVelocity.getX(), playerVelocity.getY(), playerVelocity.getZ()), deltaTime);
											break;
										}
									}
								}
							}
							else
							{
								WeaponHandler::weaponEndSound(temp->getWeapon());
							}
						}
					}
				}
				if (this->vehicleSlots->getSlot(Slots::RIGHT) != nullptr)
				{
					if (this->vehicleSlots->getSlot(Slots::RIGHT)->getObject() != nullptr)
					{
						ItemWeapon* temp = dynamic_cast<ItemWeapon*>(this->vehicleSlots->getSlot(Slots::RIGHT));
						if (this->vehicleSlots->getSlot(Slots::RIGHT)->getType() == ItemType::WEAPON && temp != nullptr)
						{
							if (Input::checkButton(Keys::L_SHOULDER, States::HELD))
							{
								//weapon.startSound();
								WeaponHandler::weaponStartSound(temp->getWeapon());
								if (temp->getWeapon().updateFireRate())
								{
									for (int i = 0; i < Vehicle::bulletCount; ++i)
									{
										if (bullets[i].getWeaponType() == WeaponType::None)
										{
											auto playerVelocity = this->vehicle->getRigidBody()->getLinearVelocity();

											Vector3 tempDir = Vector3(cos(this->vehicleBody1->getRotation().y - 3.14 / 2), 0, -sin(this->vehicleBody1->getRotation().y - 3.14 / 2));
											this->bullets[i].shoot(temp->getWeapon(),
												this->vehicleBody1->getPosition() + Vector3(right * 1.5f) + Vector3(0.0f, -0.5f, 0.0f),
												right,
												Vector3(playerVelocity.getX(), playerVelocity.getY(), playerVelocity.getZ()), deltaTime);
											break;
										}
									}
								}
							}
							else
							{
								WeaponHandler::weaponEndSound(temp->getWeapon());
							}
						}
					}
				}
				if (this->vehicleSlots->getSlot(Slots::LEFT) != nullptr)
				{
					if (this->vehicleSlots->getSlot(Slots::LEFT)->getObject() != nullptr)
					{
						ItemWeapon* temp = dynamic_cast<ItemWeapon*>(this->vehicleSlots->getSlot(Slots::LEFT));
						if (this->vehicleSlots->getSlot(Slots::LEFT)->getType() == ItemType::WEAPON && temp != nullptr)
						{
							if (Input::checkButton(Keys::L_SHOULDER, States::HELD))
							{
								//weapon.startSound();
								WeaponHandler::weaponStartSound(temp->getWeapon());
								if (temp->getWeapon().updateFireRate())
								{
									for (int i = 0; i < Vehicle::bulletCount; ++i)
									{
										if (bullets[i].getWeaponType() == WeaponType::None)
										{
											auto playerVelocity = this->vehicle->getRigidBody()->getLinearVelocity();

											Vector3 tempDir = Vector3(cos(this->vehicleBody1->getRotation().y - 3.14 / 2), 0, -sin(this->vehicleBody1->getRotation().y - 3.14 / 2));
											this->bullets[i].shoot(temp->getWeapon(), 
												this->vehicleBody1->getPosition() + Vector3(-right * 1.5f) + Vector3(0.0f, -0.5f, 0.0f),
												-right,
												Vector3(playerVelocity.getX(), playerVelocity.getY(), playerVelocity.getZ()), deltaTime);
											break;
										}
									}
								}
							}
							else
							{
								WeaponHandler::weaponEndSound(temp->getWeapon());
							}
						}
					}
				}
			}
		}

		if (dynamic_cast<PlayingGameState*>(Game::getCurrentState()) != nullptr)
		{

			for (int i = 0; i < Vehicle::bulletCount; i++)
			{
				bullets[i].update(deltaTime);
			}
			if (this->vehicleSlots->getSlot(Slots::MOUNTED) != nullptr)
			{
				if (this->vehicleSlots->getSlot(Slots::MOUNTED)->getObject() != nullptr)
				{
					this->spotLight->setPos(this->vehicleSlots->getSlot(Slots::MOUNTED)->getObject()->getPosition() - Vector3(curDir.x, -1, curDir.y));
					this->spotLight->setDirection(Vector3(curDir.x, 0, curDir.y));
				}
			}
		}

		for (int i = 0; i < Slots::SIZEOF; ++i)
		{
			ItemWeapon* itemWeapon = dynamic_cast<ItemWeapon*>(this->vehicleSlots->getSlot((Slots)i));
			if (itemWeapon != nullptr)
			{
				GameObject* weaponObject = itemWeapon->getObject();
				Weapon& weapon = itemWeapon->getWeapon();
				
				if (weapon.remainingCooldown == 0.0)
				{
					weaponObject->setColor(itemWeapon->getBaseColor() + (Vector4(1.0, 0.4, 0.1, 0.0) * (weapon.currentSpreadIncrease / weapon.maxSpread) * 2.0));
				}
				else
				{
					float time = dynamic_cast<PlayingGameState*>(Game::getCurrentState())->getTime();
					weaponObject->setColor(itemWeapon->getBaseColor() + (Vector4(0.0 + sin(time*3.0) * 0.5, 0.0 + sin(time*3.0) * 0.5, 0.0 + sin(time*3.0) * 0.5, 0.0)));
					Vector3 rotation = weaponObject->getRotation();
					Vector3 positionOffset = Vector3(sin(rotation.y), 1.0, cos(rotation.y));
					Game::getGraphics().getParticleSystem("smoke")->addParticle(2, 3, weaponObject->getPosition() + positionOffset, Vector3(0.0, 1.0, 0.0));
				}

				if (weapon.type == WeaponType::Laser)
				{
					LaserLight* laser = static_cast<LaserLight*>(weapon.light);

					if (laser != nullptr)
					{
						Vector3 rotation = weaponObject->getRotation();
						Vector3 dir = Vector3::TransformNormal(Vector3(0.0, 0.0, 1.0), Matrix::CreateFromYawPitchRoll(rotation.y, rotation.x, rotation.z));
						float heightOffset = (Slots)i == Slots::MOUNTED ? 0.0 : -0.5;

						laser->setDirection(dir);
						laser->setPos(this->vehicleBody1->getPosition() + dir * 1.5f + Vector3(0.0f, heightOffset, 0.0f));
						laser->setColor(Vector3::Lerp(weapon.lightColor, weapon.lightColor * 0.1, (weapon.currentSpreadIncrease * weapon.currentSpreadIncrease) / (weapon.maxSpread * weapon.maxSpread)));

						if (weapon.remainingCooldown > 0.0)
						{
							laser->setLuminance(0.0);

							if (weapon.remainingCooldown > 3.5)
							{
								laser->setLuminance((sin(weapon.remainingCooldown * 200.0) * sin(weapon.remainingCooldown * 111) + 1.0) * 0.05);
							}
						}
						else
						{
							if (weapon.timeSinceLastShot < 0.15)
							{
								laser->setLuminance(5.0);
							}
							else
							{
								//laser->setLuminance(0.0);
								const float laserDropOffSpeed = 20.0;
								laser->setLuminance(max(5. - (weapon.timeSinceLastShot * laserDropOffSpeed) + 0.15 * laserDropOffSpeed, 0.0));
							}
						}
					}
				}
			}
		}
	}
}

void Vehicle::setVehicleSlots(VehicleSlots* slots)
{
	if (slots == this->vehicleSlots)
		return;
	if (this->vehicleSlots != nullptr)
	{
		delete this->vehicleSlots;
	}
	this->vehicleSlots = slots;
	for (int i = 0; i < Slots::SIZEOF; i++)
	{
		Item* item = this->vehicleSlots->getSlot(Slots(i));
		if (item != nullptr)
		{
			GameObject* temp = item->getObject();
			if (temp != nullptr)
			{
				Game::getGraphics().addToDraw(temp);

				ItemWeapon* itemWeapon = static_cast<ItemWeapon*>(item);
				if (itemWeapon != nullptr)
				{
					if (itemWeapon->getWeapon().type == WeaponType::Laser)
					{
						LaserLight laser = LaserLight(
							Vector3(0,0,0),
							itemWeapon->getWeapon().lightColor,
							0.0,
							Vector3(0,0,0),
							itemWeapon->getWeapon().bulletScale.z);
						itemWeapon->getWeapon().light = (Light*)static_cast<PlayingGameState*>(Game::getCurrentState())->addLight(laser);
					}
				}
			}
		}
	}
}

void Vehicle::setSpecSlot(Slots slot, Item* item)
{
	if (this->vehicleSlots->getSlot(slot) != nullptr)
	{
		Game::getGraphics().removeFromDraw(this->vehicleSlots->getSlot(slot)->getObject());
	}
	if (item!=nullptr)
	{
		if (item->getObject() != nullptr)
		{
			Game::getGraphics().addToDraw(item->getObject());
		}
	}
	this->vehicleSlots->setSlot(slot, item);
}

VehicleSlots* Vehicle::getSlots()
{
	return this->vehicleSlots;
}

float Vehicle::getAcceleratorX()
{
	return accelerator.x;
}

void Vehicle::startEngineSound()
{
	soundHandle = Sound::playLooping("./data/sound/CarEngine4.mp3",0.5f);
}

Stats Vehicle::getStats() const
{
	return this->updatedStats;
}

void Vehicle::setSpotLight(SpotLight* theLight)
{
	this->spotLight = theLight;
}

SpotLight* Vehicle::getSpotLight()
{
	return this->spotLight;
}

void Vehicle::setLaserLight(LaserLight* light)
{
	//this->laserLight = light;
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
	return Vector3(velocity.x, 0, velocity.y);
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
	if (!this->immortal && powerUpTimers[(int)PowerUpType::Star] <= 0.0)
	{
		if (amount < 0) {
			dmg = true;
		}
		vehicleBody1->setColor(Vector4(max(vehicleBody1->getColor().x + -amount * 0.6f, 0), vehicleBody1->getColor().y, vehicleBody1->getColor().z, 1));
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

void Vehicle::makePlayer()
{
	this->player = true;
}

bool Vehicle::isPlayer() const
{
	return this->player;
}

float Vehicle::getTotalRespawnTime() const
{
	return this->totalRespawnTime;
}

float Vehicle::getRespawnTimer() const
{
	return respawnTimer;
}

Vector3 Vehicle::getCameraDistance(float deltaTime)
{
	float vehicleDistance = 0;
	Vector3 vehicleCameraDistance = Vector3(this->vehicle->getRigidBody()->getLinearVelocity().getX(), this->vehicle->getRigidBody()->getLinearVelocity().getY(), this->vehicle->getRigidBody()->getLinearVelocity().getZ());
	vehicleDistance = max(abs(vehicleCameraDistance.x), abs(vehicleCameraDistance.z)) * 0.7f;

	if (vehicleDistance < 10.0f) {
		vehicleDistance = 0.0f;
	}
	vehicleDistance = min(vehicleDistance, 20.0f);

	aimLerp = Vector2::Lerp(aimLerp, Vector2(Input::getDirectionRnoMouse().x * Input::getStrengthRnoMouse() * 3, Input::getDirectionRnoMouse().y * Input::getStrengthRnoMouse() * 3), deltaTime*10.0f);

	cameraDistance = (vehicleDistance - cameraDistance) * deltaTime * 1.2f + cameraDistance;
	cameraDistanceX = ((this->vehicle->getRigidBody()->getLinearVelocity().getX() * 0.3f + aimLerp.x) - cameraDistanceX) * deltaTime * 12.2f + cameraDistanceX;
	cameraDistanceZ = ((this->vehicle->getRigidBody()->getLinearVelocity().getZ() * 0.25f + aimLerp.y) - cameraDistanceZ) * deltaTime * 12.2f + cameraDistanceZ;

	return Vector3(cameraDistanceX, cameraDistance, cameraDistanceZ);
}

void Vehicle::setAccelForce(Vector3 accelForce, float deltaTime)
{

	//AccelForce
	if ((max(abs(accelForce.x), abs(accelForce.z)) > 5.0f)) {
		int randomSound = rand() % 3 + 1;
		std::string soundEffect = "./data/sound/CarImpact" + std::to_string(randomSound) + ".wav";
		int randomSound2 = rand() % 3 + 1;
		std::string soundEffect2 = "./data/sound/MetalImpactPitched" + std::to_string(randomSound) + ".wav";
		if (max(abs(accelForce.x), abs(accelForce.z)) > 25.0f) {
			for (int i = 0; i < 10; i++) {
				Game::getGraphics().getParticleSystem("debris")->addParticle(1, 10.5f, this->vehicle->getPosition() + Vector3(0, 2, 0), Vector4(this->vehicle->getRigidBody()->getLinearVelocity().getX(), 1.0f, this->vehicle->getRigidBody()->getLinearVelocity().getZ(), 1));
			}
			changeHealth(-20.0f);
			Sound::play("./data/sound/CarCrash.wav");
			Sound::play(soundEffect2);
		}
		else if (max(abs(accelForce.x), abs(accelForce.z)) > 15.0f) {
			for (int i = 0; i < 5; i++) {
				Game::getGraphics().getParticleSystem("debris")->addParticle(1, 10.5f, this->vehicle->getPosition() + Vector3(0, 2, 0), Vector4(this->vehicle->getRigidBody()->getLinearVelocity().getX(), 1.0f, this->vehicle->getRigidBody()->getLinearVelocity().getZ(), 1));
			}
			changeHealth(-10.0f);
			Sound::play(soundEffect);
			Sound::play(soundEffect2);
		}
		else {
			for (int i = 0; i < 2; i++) {
				Game::getGraphics().getParticleSystem("debris")->addParticle(1, 10.5f, this->vehicle->getPosition() + Vector3(0, 2, 0), Vector4(this->vehicle->getRigidBody()->getLinearVelocity().getX(), 1.0f, this->vehicle->getRigidBody()->getLinearVelocity().getZ(), 1));
			}
			Sound::play("./data/sound/CarImpactSoft.wav");
			Sound::play(soundEffect2);
		}
	}
	/*else {
		vehicleBody1->setColor(Vector4(vehicleBody1->getColor().x/(1 + 20.0f*deltaTime), vehicleBody1->getColor().y / (1 + 20.0f * deltaTime), vehicleBody1->getColor().z / (1 + 20.0f * deltaTime), 1));
	}*/
}

void Vehicle::setWheelRotation(float deltaTime)
{
	Quaternion qt = Vector4(this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getX(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getY(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getZ(), this->vehicleBody1->getRigidBody()->getWorldTransform().getRotation().getW());
	float bodyHeading = getHeading(qt);
	//bodyHeading = fmod((bodyHeading * 180 / DirectX::XM_PI), 360.0f);


	Vector2 wheel1Pos = Vector2(vehicleBody1->getPosition().x + (cos(-bodyHeading) * 0.41f) + (-sin(-bodyHeading) * 0.73f),
								vehicleBody1->getPosition().z + (sin(-bodyHeading) * 0.41f) + (cos(-bodyHeading) * 0.73f));
	Vector2 wheel2Pos = Vector2(vehicleBody1->getPosition().x + (cos(bodyHeading) * -0.41f) + (-sin(bodyHeading) * -0.73f),
								vehicleBody1->getPosition().z + (sin(bodyHeading) * 0.41f) + (cos(bodyHeading) * 0.73f));
	Vector2 wheel3Pos = Vector2(vehicleBody1->getPosition().x + (cos(-bodyHeading) * -0.41f) + (-sin(-bodyHeading) * -0.68f),
								vehicleBody1->getPosition().z + (sin(-bodyHeading) * -0.41f) + (cos(-bodyHeading) * -0.68f));
	Vector2 wheel4Pos = Vector2(vehicleBody1->getPosition().x + (cos(bodyHeading) * 0.41f) + (-sin(bodyHeading) * 0.68f),
								vehicleBody1->getPosition().z + (sin(bodyHeading) * -0.41f) + (cos(bodyHeading) * -0.68f));

	wheel1->setPosition(Vector3(wheel1Pos.x, vehicleBody1->getPosition().y -0.25f, wheel1Pos.y));
	wheel2->setPosition(Vector3(wheel2Pos.x, vehicleBody1->getPosition().y - 0.25f, wheel2Pos.y));
	wheel3->setPosition(Vector3(wheel3Pos.x, vehicleBody1->getPosition().y - 0.25f, wheel3Pos.y));
	wheel4->setPosition(Vector3(wheel4Pos.x, vehicleBody1->getPosition().y - 0.25f, wheel4Pos.y));

	Quaternion rotationNow = XMQuaternionRotationRollPitchYaw(wheel1->getRotation().x, wheel1->getRotation().y, wheel1->getRotation().z);
	Quaternion rotationDest = Quaternion::Lerp(rotationNow, Quaternion::CreateFromAxisAngle(Vector3(0, 1, 0), std::atan2f(Input::getDirectionL().x, Input::getDirectionL().y)), deltaTime *20);
	rotationNow.Normalize();
	rotationDest.Normalize();

	float dotProduct = rotationNow.x * rotationDest.x + rotationNow.y * rotationDest.y + rotationNow.z * rotationDest.z + rotationNow.w * rotationDest.w;

	if (Input::getStrengthL() < 0.01f) {
		rotationDest = Quaternion::Lerp(rotationNow, Quaternion::CreateFromAxisAngle(Vector3(0, 1, 0), bodyHeading), deltaTime * 20);
	}
	else {
		float angle1 = bodyHeading;
		float angle2 = getHeading(rotationNow);
		float a = abs(angle1 - angle2);
		fmod((a * 180 / DirectX::XM_PI), 360.0f);
		if (abs(a) > 45 * (DirectX::XM_PI / 180)) {
			rotationDest = Quaternion::Lerp(rotationNow, Quaternion::CreateFromAxisAngle(Vector3(0, 1, 0), bodyHeading), deltaTime * 20);
		}
	}
	float wheelHeading = getHeading(rotationDest);

	wheel1->setRotation(Vector3(0, wheelHeading, 0));
	wheel2->setRotation(Vector3(0, wheelHeading, 0));
	wheel3->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
	wheel4->setRotation(Vector3(0, vehicleBody1->getRotation().y + XM_PI, 0));
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

void Vehicle::addPowerUp(PowerUpType type)
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
	else if (type == PowerUpType::Star)
	{
		Sound::changeSoundtrackVolume(0);
		Sound::stopLooping(starPowerHandle);
		starPowerHandle = Sound::playLooping("./data/sound/StarPowerup.mp3", 0.75f);
		this->powerUpTimers[(int)PowerUpType::Star] += 30.0;
	}
}

void Vehicle::updatePowerUpEffects(float deltaTime)
{
	// star power up
	if (powerUpTimers[(int)PowerUpType::Star] > 0.0)
	{
		this->updatedStats.accelerationRate = this->defaultStats.accelerationRate * 3.0f;
		this->updatedStats.handlingRate = this->defaultStats.handlingRate * 1.3f;

		// animation
		float time = dynamic_cast<PlayingGameState*>(Game::getCurrentState())->getTime();
		this->vehicleBody1->setColor(Vector4(fmod(time * 3.14, 1.0), fmod(time * 2.33, 1.0), fmod(time * 2.81, 1.0), 1.0));
		btVector3 btVelocity = vehicle->getRigidBody()->getLinearVelocity();
		Vector3 velocity = Vector3(btVelocity.getX(), btVelocity.getY(), btVelocity.getZ());
		Game::getGraphics().addParticle(this->vehicleBody1->getPosition(),
			Vector3(0.0, 10.0, 0.0) + velocity,
			1,
			10.5f,
			11.f);
	}
	// speed power up
	else if (powerUpTimers[(int)PowerUpType::Speed] > 0.0)
	{
		this->updatedStats.accelerationRate = this->defaultStats.accelerationRate * 3.0f;
		this->vehicleBody1->setColor(Vector4(0.0, 1.0, 0.0, 1.0));
	}
	// reset stats if no power up
	else
	{
		this->updatedStats.accelerationRate = this->defaultStats.accelerationRate;
		this->updatedStats.handlingRate = this->defaultStats.handlingRate;
		//this->vehicleBody1->setColor(Vector4(0.0, 0.0, 0.0, 1.0));
		Sound::changeSoundtrackVolume(1);
		Sound::stopLooping(starPowerHandle);
	}

	for (int i = 0; i < (int)PowerUpType::Length; ++i)
	{
		powerUpTimers[i] = max(powerUpTimers[i] - deltaTime, 0.0f);
	}
}

float Vehicle::getPowerUpTimer(PowerUpType p)
{
	return this->powerUpTimers[(int)p];
}
