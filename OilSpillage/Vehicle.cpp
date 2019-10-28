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
	this->accelForce = Vector3(0,0,0);
	this->reverseTimer = 0;
	this->reverseTimer2 = 0;

	this->curDir = Vector2(0.0f, 1.0f);//UP
	this->timeSinceLastShot = 0.0f;
	this->weapon = WeaponHandler::getWeapon(WeaponType::MachineGun);
	this->defaultStats = VehicleStats::fastCar;
	this->updatedStats = this->defaultStats;
	this->health = this->updatedStats.maxHealth;
}

Vehicle::~Vehicle()
{
	delete vehicle;
	delete vehicleBody1;

	delete this->mountedWeapon;
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
	this->vehicle = new GameObject;
	vehicle->mesh = Game::getGraphics().getMeshPointer("Cube");
	//Game::getGraphics().addToDraw(vehicle);
	vehicle->setPosition(Vector3(0.0f, 10.0f, 0.0f));
	vehicle->setScale(Vector3(0.5f, 0.14f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp");
	vehicle->setTexture(Game::getGraphics().getTexturePointer("CarTemp"));

	mountedWeapon->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Turret1");
	Material mountedWPMaterial = Game::getGraphics().getMaterial("Entities/Dummy_Turret");
	mountedWeapon->setTexture(mountedWPMaterial.diffuse);
	Game::getGraphics().addToDraw(mountedWeapon);
	mountedWeapon->setScale(Vector3(0.005f));

	this->vehicleBody1 = new GameObject;
	vehicleBody1->mesh = Game::getGraphics().getMeshPointer("Entities/Dummy_Player_Car1");
	Game::getGraphics().addToDraw(vehicleBody1);
	vehicleBody1->setScale(Vector3(0.005f, 0.005f, 0.005f));	
	//vehicleBody1->setPosition(Vector3(0.0f, 0.65f, 0.0f));
	//vehicleBody1->setScale(Vector3(0.5f, 0.22f, 0.9f));

	vehicleBody1->setTexture(Game::getGraphics().getMaterial("Entities/Dummy_Player_Car").diffuse);

	this->wheel1 = new GameObject;
	this->wheel2 = new GameObject;
	this->wheel3 = new GameObject;
	this->wheel4 = new GameObject;
	wheel1->mesh = Game::getGraphics().getMeshPointer("Cube");
	wheel2->mesh = Game::getGraphics().getMeshPointer("Cube");
	wheel3->mesh = Game::getGraphics().getMeshPointer("Cube");
	wheel4->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(wheel1);
	Game::getGraphics().addToDraw(wheel2);
	Game::getGraphics().addToDraw(wheel3);
	Game::getGraphics().addToDraw(wheel4);
	wheel1->setPosition(Vector3(1.0f, 2.0f, 1.0f));
	wheel2->setPosition(Vector3(-1.0f, 2.0f, 1.0f));
	wheel3->setPosition(Vector3(-1.0f, 2.0f, -1.0f));
	wheel4->setPosition(Vector3(1.0f, 2.0f, -1.0f));
	wheel1->setScale(Vector3(0.2f, 0.2f, 0.2f));
	wheel2->setScale(Vector3(0.2f, 0.2f, 0.2f));
	wheel3->setScale(Vector3(0.2f, 0.2f, 0.2f));
	wheel4->setScale(Vector3(0.2f, 0.2f, 0.2f));
	wheel1->setColor(Vector4(0.0f, 0.0f, 1.0f, 1.0f)); //Top Right
	wheel2->setColor(Vector4(0.0f, 1.0f, 0.0f, 1.0f)); //Top Left
	wheel3->setColor(Vector4(1.0f, 0.0f, 0.0f, 1.0f)); //Bottom Left
	wheel4->setColor(Vector4(1.0f, 1.0f, 0.0f, 1.0f)); //Bottom Right

	btRigidBody* tempo = physics->addBox(btVector3(vehicle->getPosition().x, vehicle->getPosition().y, vehicle->getPosition().z), btVector3(vehicle->getScale().x, vehicle->getScale().y, vehicle->getScale().z), 10.0f);
	this->getVehicle()->setRigidBody(tempo, physics);
	this->getVehicle()->getRigidBody()->activate();
	this->getVehicle()->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	this->getVehicle()->getRigidBody()->setFriction(0);

	tempo = physics->addBox(btVector3(vehicle->getPosition().x, vehicle->getPosition().y + 0.65f, vehicle->getPosition().z), -btVector3(this->vehicleBody1->getAABB().minPos.x - this->vehicleBody1->getAABB().maxPos.x, (this->vehicleBody1->getAABB().minPos.y - this->vehicleBody1->getAABB().maxPos.y) * 0.2f, this->vehicleBody1->getAABB().minPos.z - this->vehicleBody1->getAABB().maxPos.z) * 0.5f, 1.0f);
	vehicleBody1->setRigidBody(tempo, physics);
	vehicleBody1->getRigidBody()->activate();
	vehicleBody1->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	vehicleBody1->getRigidBody()->setFriction(1);

	tempo = physics->addSphere(0.2f, btVector3(wheel1->getPosition().x, wheel1->getPosition().y, wheel1->getPosition().z),10.0f);
	wheel1->setRigidBody(tempo, physics);
	tempo = physics->addSphere(0.2f, btVector3(wheel2->getPosition().x, wheel2->getPosition().y, wheel2->getPosition().z), 10.0f);
	wheel2->setRigidBody(tempo, physics);
	tempo = physics->addSphere(0.2f, btVector3(wheel3->getPosition().x, wheel3->getPosition().y, wheel3->getPosition().z), 10.0f);
	wheel3->setRigidBody(tempo, physics);
	tempo = physics->addSphere(0.2f, btVector3(wheel4->getPosition().x, wheel4->getPosition().y, wheel4->getPosition().z), 10.0f);
	wheel4->setRigidBody(tempo, physics);

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

void Vehicle::update(float deltaTime)
{

	for (int i = 0; i < (int)PowerUpType::Length; ++i)
	{
		powerUpTimers[i] = max(powerUpTimers[i] - deltaTime, 0.0f);
	}
	if (powerUpTimers[(int)PowerUpType::Speed] > 0.0)
	{
		this->updatedStats.accelerationRate = this->defaultStats.accelerationRate * 3.0f;
		this->vehicle->setColor(Vector4(1.0, 0.0, 0.0, 1.0));
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
	float difference = (180 - abs(abs(bodyHeading - heading) - 180))*0.25f; ;
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
		if (Input::checkButton(Keys::R_TRIGGER, States::HELD) || Input::isKeyDown_DEBUG(Keyboard::W)) {
			if (velocitySpeed < (40 * updatedStats.maxSpeed)) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(dx* deltaTime * 160.0f * updatedStats.accelerationRate, 0, -(dy * deltaTime * 160.0f * updatedStats.accelerationRate)), btVector3(0, 0, 0));
			}
		}
		if (Input::getDirectionL().x > 0) {
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
		else if (Input::getDirectionL().x < 0) {
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
			vehicle->getRigidBody()->setAngularVelocity(btVector3(steering.x, vehicle->getRigidBody()->getAngularVelocity().getY()/ (1 + (7.0f * deltaTime)), steering.z));

		}
		if ((Input::checkButton(Keys::L_TRIGGER, States::HELD) || Input::isKeyDown_DEBUG(Keyboard::S)) && velocitySpeed > (-40 * updatedStats.maxSpeed)) {
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
		if (Input::getStrengthL() > 0) {
			if (velocitySpeed < 0.5f) {
				reverseTimer += 10.0f * deltaTime;
			}
			else {
				reverseTimer = 0;
				reverseTimer2 = 0;
			}
			if ((reverseTimer > 10.0f) && !(reverseTimer2 > 10.0f)) {
				this->vehicle->getRigidBody()->applyImpulse(btVector3(-dx* deltaTime * 160.0f * updatedStats.accelerationRate, 0, (dy * deltaTime * 160.0f * updatedStats.accelerationRate)), btVector3(0, 0, 0));
				reverseTimer2 += 10.0f * deltaTime;
				if (reverseTimer2 > 10.0f) {
					reverseTimer = 0;
					reverseTimer2 = 0;
				}
			}
			else {
				reverseTimer2 = 0;
				if (velocitySpeed < (40 * updatedStats.maxSpeed)) {
					this->vehicle->getRigidBody()->applyImpulse(btVector3(dx * deltaTime * 160.0f * updatedStats.accelerationRate, 0, -(dy * deltaTime * 160.0f * updatedStats.accelerationRate)), btVector3(0, 0, 0));
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
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, steering.y* difference2, 0));
				}
				else {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, -steering.y * difference2, 0));
				}
			}
			else {
				if (abs(vehicleRotation - targetRotation) < 180) {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, -steering.y * difference2, 0));
				}
				else {
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, steering.y* difference2, 0));
				}
			}
		}
		else {
			vehicle->getRigidBody()->setLinearVelocity(btVector3(vehicle->getRigidBody()->getLinearVelocity().getX() / (1 + (1.8f * deltaTime)), vehicle->getRigidBody()->getLinearVelocity().getY(), vehicle->getRigidBody()->getLinearVelocity().getZ() / (1 + (1.8f * deltaTime))));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 40;
			//vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime*2 * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
			vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 120 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
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


	add += 0.4f * deltaTime;

	if (dmg == true) {
		vehicleBody1->setColor(Vector4(vehicleBody1->getColor().x / (1 + 15.0f * deltaTime), vehicleBody1->getColor().y / (1 + 15.0f * deltaTime), vehicleBody1->getColor().z / (1 + 15.0f * deltaTime), 1));
		if (vehicleBody1->getColor().x <= 0.01f) {
			dmg = false;
		}
	}
}

void Vehicle::updateWeapon(float deltaTime)
{
	this->mountedWeapon->setPosition(this->vehicleBody1->getPosition());
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

	float newRot = atan2(curDir.x, curDir.y) + 3.14f / 2;
	this->gunRotation = newRot;
	this->mountedWeapon->setRotation(Vector3(0, newRot, 0));

	this->timeSinceLastShot += deltaTime;
	// recoil goes from 100% to 0% in half a second
	this->weapon.currentSpreadIncrease = max(this->weapon.currentSpreadIncrease - deltaTime * this->weapon.maxSpread * 2.0, 0.0);

	if (Input::checkButton(Keys::R_SHOULDER, States::HELD) || Input::getStrengthR() > 0.01f)
	{
		if (this->timeSinceLastShot >= this->weapon.fireRate)
		{
			this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);
			
			for (int i = 0; i < Vehicle::bulletCount; ++i)
			{
				if (bullets[i].getWeaponType() == WeaponType::None)
				{
					auto playerVelocity = this->vehicle->getRigidBody()->getLinearVelocity();

					this->bullets[i].shoot(weapon,
										   this->vehicleBody1->getPosition() + Vector3(0, 0, 0),
						                   Vector3(curDir.x, 0.0, curDir.y),
						                   Vector3(playerVelocity.getX(), playerVelocity.getY(), playerVelocity.getZ()));
					break;
				}
			}
		}
	}

	for (int i = 0; i < Vehicle::bulletCount; i++)
	{
		bullets[i].update(deltaTime);
	}
}

float Vehicle::getAcceleratorX()
{
	return accelerator.x;
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
	if (amount < 0) {
		dmg = true;
	}
	vehicleBody1->setColor(Vector4(max(vehicleBody1->getColor().x + -amount*0.1f,0), 0, 0, 1));
	this->health = std::clamp(this->health + amount, 0, this->updatedStats.maxHealth);
	Game::getGraphics().addParticle2(this->vehicle->getPosition(), Vector3(0, 0, 0), 2, 1);
}

bool Vehicle::isDead() const
{
	return this->health <= 0;
}

float Vehicle::getCameraDistance(float deltaTime)
{
	float vehicleDistance = 0;
	Vector3 vehicleCameraDistance = Vector3(this->vehicle->getRigidBody()->getLinearVelocity().getX(), this->vehicle->getRigidBody()->getLinearVelocity().getY(), this->vehicle->getRigidBody()->getLinearVelocity().getZ());
	vehicleDistance = max(abs(vehicleCameraDistance.x), abs(vehicleCameraDistance.z)) *0.7f;

	if (vehicleDistance < 10.0f) {
		vehicleDistance = 0.0f;
	}
	vehicleDistance = min(vehicleDistance, 20.0f);

	cameraDistance = (vehicleDistance - cameraDistance) * deltaTime*1.2f + cameraDistance;

	return cameraDistance;
}

void Vehicle::setAccelForce(Vector3 accelForce, float deltaTime)
{
	//AccelForce
	if (max(abs(accelForce.x), abs(accelForce.z)) > 15.0f) {
		Game::getGraphics().addParticle2(this->vehicle->getPosition(), Vector3(0, 0, 0), 2, 1);
		changeHealth(-20.0f);
	}
	else if (max(abs(accelForce.x), abs(accelForce.z)) > 5.0f) {
		Game::getGraphics().addParticle2(this->vehicle->getPosition(), Vector3(0, 0, 0), 2, 1);
		changeHealth(-10.0f);
	}
	/*else {
		vehicleBody1->setColor(Vector4(vehicleBody1->getColor().x/(1 + 20.0f*deltaTime), vehicleBody1->getColor().y / (1 + 20.0f * deltaTime), vehicleBody1->getColor().z / (1 + 20.0f * deltaTime), 1));
	}*/
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
