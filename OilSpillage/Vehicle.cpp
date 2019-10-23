#include "Vehicle.h"
#include"game.h"
#include "Input.h"

Vehicle::Vehicle()
{
	velocity = Vector2(0.0f, 0.0f);
	accelerator = Vector3(0.0f, 0.0f, 0.0f);
	acceleratorTempX = 0;
	acceleratorTempZ = 0;
	strength = 0;

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
	this->bodyRotation = nullptr;
	this->bodyRotationPoint = nullptr;

	this->timeSinceLastShot = 0.0f;
	this->weapon = WeaponHandler::getWeapon(WeaponType::Flamethrower);
	this->defaultStats = VehicleStats::fastCar;
	this->updatedStats = this->defaultStats;

	this->health = this->updatedStats.maxHealth;
}

Vehicle::~Vehicle()
{
	delete vehicle;
	delete bodyRotation;
	delete bodyRotationPoint;

	delete wheel1;
	delete wheel2;
	delete wheel3;
	delete wheel4;
	delete spring1;
}

void Vehicle::init(Physics *physics)
{
	this->physics = physics;

	this->vehicle = new GameObject;
	vehicle->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(vehicle);
	vehicle->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	vehicle->setScale(Vector3(0.5f, 0.4f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp");
	vehicle->setTexture(Game::getGraphics().getTexturePointer("CarTemp"));

	/*this->bodyRotation = new GameObject;
	bodyRotation->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(bodyRotation);
	bodyRotation->setPosition(Vector3(0.0f, 0.2f, 0.0f));
	bodyRotation->setScale(Vector3(0.5f, 0.12f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp");
	bodyRotation->setTexture(Game::getGraphics().getTexturePointer("CarTemp"));*/

	this->bodyRotationPoint = new GameObject;
	bodyRotationPoint->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(bodyRotationPoint);
	bodyRotationPoint->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	bodyRotationPoint->setScale(Vector3(0.2f, 0.2f, 0.2f));
	Game::getGraphics().loadTexture("brickwall");
	bodyRotationPoint->setTexture(Game::getGraphics().getTexturePointer("brickwall"));

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

	btRigidBody* tempo = physics->addBox(btVector3(0, 0, 0), btVector3(this->getVehicle()->getScale().x, this->getVehicle()->getScale().y, this->getVehicle()->getScale().z), 10.0f);
	this->getVehicle()->setRigidBody(tempo, physics);
	this->getVehicle()->getRigidBody()->activate();
	this->getVehicle()->getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	this->getVehicle()->getRigidBody()->setFriction(0);

	tempo = physics->addSphere(0.2f, btVector3(wheel1->getPosition().x, wheel1->getPosition().y, wheel1->getPosition().z),10.0f);
	wheel1->setRigidBody(tempo, physics);
	tempo = physics->addSphere(0.2f, btVector3(wheel2->getPosition().x, wheel2->getPosition().y, wheel2->getPosition().z), 10.0f);
	wheel2->setRigidBody(tempo, physics);
	tempo = physics->addSphere(0.2f, btVector3(wheel3->getPosition().x, wheel3->getPosition().y, wheel3->getPosition().z), 10.0f);
	wheel3->setRigidBody(tempo, physics);
	tempo = physics->addSphere(0.2f, btVector3(wheel4->getPosition().x, wheel4->getPosition().y, wheel4->getPosition().z), 10.0f);
	wheel4->setRigidBody(tempo, physics);

	spring1 = new btGeneric6DofSpring2Constraint(
		*this->vehicle->getRigidBody(), *wheel1->getRigidBody(), 
		btTransform(btQuaternion::getIdentity(), { 0.0f, -1.0f, 0.0f }),
		btTransform(btQuaternion::getIdentity(), { 0.0f,  0.0f, 0.0f })
	);
	/*spring1->setLinearLowerLimit(btVector3(0.0f, 1.0f, 0.0f));
	spring1->setLinearUpperLimit(btVector3(0.0f, 0.0f, 0.0f));*/
	spring1->enableSpring(1, true);
	spring1->setStiffness(1, 35.0f);
	spring1->setDamping(1, 0.5f);
	spring1->setEquilibriumPoint();
	

	bodyPivot = Vector3(0.0f, 1.2f, 0.0f);
}

void Vehicle::update(float deltaTime)
{
	deltaTime *= 2;

	tempTargetRotation = targetRotation;

	//Quaternion Rotation to Euler
	btScalar m;
	btScalar n;
	Quaternion qt = Vector4(this->vehicle->getRigidBody()->getWorldTransform().getRotation().getX(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getY(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getZ(), this->vehicle->getRigidBody()->getWorldTransform().getRotation().getW());

	const btVector3 UP(0.0, 1.0, 0.0);
	const btVector3 RIGHT(1.0, 0.0, 0.0);
	const btVector3 FORWARD(0.0, 0.0, -1.0);

	btQuaternion finalOrientation;
	btVector3 forward;

	const btScalar MIN_USEABLE_LENGTH_SQUARED = 1.0e-8;
	btScalar forward2 = forward.length2();
	if (forward2 > MIN_USEABLE_LENGTH_SQUARED) {
		forward /= sqrt(forward2);  // forward is now normalized
		btScalar upComponent = forward.dot(UP);
		btScalar pitch = asin(upComponent);
		btVector3 horizontal = forward - upComponent * UP;
		btScalar yaw = atan2(horizontal.dot(RIGHT), horizontal.dot(FORWARD));

		// yaw and pitch are only valid in this context, not in the else case below
		// compute finalOrientation
		btQuaternion yawRotation(UP,yaw);
		btQuaternion pitchRotation(RIGHT, pitch);
		finalOrientation = yawRotation * pitchRotation;
		qt = Vector4(finalOrientation.getX(), finalOrientation.getY(), finalOrientation.getZ(), finalOrientation.getW());
	}
	else {
		// bad data
	}
	XMVECTOR temp3;
	temp3.m128_f32[0] = this->vehicle->getRotationQuaternion().x;
	temp3.m128_f32[1] = this->vehicle->getRotationQuaternion().y;
	temp3.m128_f32[2] = this->vehicle->getRotationQuaternion().z;
	temp3.m128_f32[3] = this->vehicle->getRotationQuaternion().w;

	qt = this->vehicle->getRotationQuaternion();

	float heading;
	float attitude;
	float bank;

	double test = qt.x * qt.y + qt.z * qt.w;
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
		double sqx = qt.x * qt.x;
		double sqy = qt.y * qt.y;
		double sqz = qt.z * qt.z;
		heading = atan2(2 * qt.y * qt.w - 2 * qt.x * qt.z, 1 - 2 * sqy - 2 * sqz);
		attitude = asin(2 * test);
		bank = atan2(2 * qt.x * qt.w - 2 * qt.y * qt.z, 1 - 2 * sqx - 2 * sqz);
	}

	this->vehicle->getRigidBody()->getWorldTransform().getRotation().getEulerZYX(n,mRotation,m);
	//if (mRotation < 0) mRotation += 180;

	//Current rotation of vehicle
	float vehicleRotation = fmod((/*(getYaw(qt)*/heading * 180 / DirectX::XM_PI), 360.0f);
	if (vehicleRotation < 0)
		vehicleRotation += 360;
	mRotation = vehicleRotation;
	
	//Direction of Vehicle in dx & dy
	float dx = sin((DirectX::XM_PI / 180) * vehicleRotation);
	float dy = -cos((DirectX::XM_PI / 180)* vehicleRotation);


	targetRotation = (atan2(-Input::GetDirectionL(0).x, -Input::GetDirectionL(0).y) * 180 / DirectX::XM_PI) + 180;

	velocitySpeed = (velocity.x * (dx)+velocity.y * (dy));
	//Driving mode: Turn towards direction, semi-realistic
	if (drivingMode == 0) {	
		if (Input::GetStrengthL(0) > 0) {
			if (velocitySpeed < (5000 * updatedStats.maxSpeed)) {
				this->velocity.x += dx *deltaTime* 800 * Input::GetStrengthL(0) * updatedStats.accelerationRate;
				this->velocity.y += dy *deltaTime* 800 * Input::GetStrengthL(0) * updatedStats.accelerationRate;
			}

			if (strength < Input::GetStrengthL(0)) {
				strength = Input::GetStrengthL(0);
			}
			else {
				strength -= 0.01f * deltaTime;
			}

			Vector2 targetDir(Input::GetDirectionL(0));

			if ((targetDir - this->currentDir).Length() > 0.01f)
			{
				this->currentDir = Vector2::Lerp(this->currentDir, targetDir, deltaTime * min(4* min(velocitySpeed*0.0003f,1),2));
				this->currentDir.Normalize();
			}
			else
			{
				this->currentDir = targetDir;
			}

			float newRot = atan2(this->currentDir.x, this->currentDir.y);
			vehicle->getRigidBody()->setAngularVelocity(btVector3(0, newRot, 0));

		}
		else {
			this->velocity.x = (velocity.x / (1 + (0.001f * 3000 * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (0.001f * 3000 * deltaTime)));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 20;
			vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
		}

		rotateAcceleration /= 1.0f + 0.002f*deltaTime*1500;
		/*if (abs(rotateAcceleration) < 0.5) {
			rotateAcceleration = 0;
		}*/
	}
	//Driving mode: Throttle and turning, realistic
	else if (drivingMode == 1) {
		if (Input::CheckButton(Keys::R_TRIGGER, HELD, 0) || Input::IsKeyDown_DEBUG(Keyboard::W)) {
			if (velocitySpeed < (5000 * updatedStats.maxSpeed)) {
				this->velocity.x += dx * deltaTime * 800 * Input::GetStrengthL(0) * updatedStats.accelerationRate;
				this->velocity.y += dy * deltaTime * 800 * Input::GetStrengthL(0) * updatedStats.accelerationRate;
			}
		}
		if (Input::GetDirectionL(0).x > 0) {
			if (rotationDirection == true) {
				rotateAcceleration = 0;
			}
			if (velocity.x * dx > 0.0f || velocity.y * dy > 0.0f) {
				rotateAcceleration += deltaTime * 5 * ((abs(velocity.x) + abs(velocity.y)) / 5000);
				if (rotateAcceleration > 3.04f * ((abs(velocity.x) + abs(velocity.y)) / 14000)) {
					rotateAcceleration = 3.04f * ((abs(velocity.x) + abs(velocity.y)) / 14000);
				}
				if (rotateAcceleration > 2.6f * Input::GetStrengthL(0)) {
					rotateAcceleration = 2.6f * Input::GetStrengthL(0);
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(0, Input::GetStrengthL(0)* ((((0.19f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 8000 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				rotationDirection = false;
			}
			else {
				rotateAcceleration -= deltaTime * 5 * ((abs(velocity.x) + abs(velocity.y)) / 5000);
				if (rotateAcceleration < -3.04f * ((abs(velocity.x) + abs(velocity.y)) / 14000)) {
					rotateAcceleration = -3.04f * ((abs(velocity.x) + abs(velocity.y)) / 14000);
				}
				if (rotateAcceleration < -2.6f * Input::GetStrengthL(0)) {
					rotateAcceleration = -2.6f * Input::GetStrengthL(0);
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(0, Input::GetStrengthL(0)* ((((-0.19f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 8000 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				rotationDirection = false;
			}
		}
		else if (Input::GetDirectionL(0).x < 0) {
			if (rotationDirection == false) {
				rotateAcceleration = 0;
			}
			if (velocity.y * dy > 0.0f || velocity.x * dx > 0.0f) {
				rotateAcceleration -= deltaTime * 5 * ((abs(velocity.x) + abs(velocity.y)) / 5000);
				if (rotateAcceleration < -3.04f * ((abs(velocity.x) + abs(velocity.y)) / 14000)) {
					rotateAcceleration = -3.04f * ((abs(velocity.x) + abs(velocity.y)) / 14000);
				}
				if (rotateAcceleration < -2.6f * Input::GetStrengthL(0)) {
					rotateAcceleration = -2.6f * Input::GetStrengthL(0);
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(0, Input::GetStrengthL(0)* ((((-0.19f) + rotateAcceleration * 2) / 3) * DirectX::XM_PI / 180) * deltaTime * 8000 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				rotationDirection = true;
			}
			else {
				rotateAcceleration += deltaTime * 5 * ((abs(velocity.x) + abs(velocity.y)) / 5000);
				if (rotateAcceleration > 3.04f * ((abs(velocity.x) + abs(velocity.y)) / 14000)) {
					rotateAcceleration = 3.04f * ((abs(velocity.x) + abs(velocity.y)) / 14000);
				}
				if (rotateAcceleration > 2.6f * Input::GetStrengthL(0)) {
					rotateAcceleration = 2.6f * Input::GetStrengthL(0);
				}
				vehicle->getRigidBody()->setAngularVelocity(btVector3(0, Input::GetStrengthL(0)* ((((0.19f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 8000 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				rotationDirection = true;
			}
		}
		else {
			//rotateAcceleration = 0;
			rotateAcceleration /= 1.0f + 10 * deltaTime;
			vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 8000 * ((abs(velocity.x) + abs(velocity.y)) / 8000), 0));

		}
		if (Input::CheckButton(Keys::L_TRIGGER, HELD, 0) || Input::IsKeyDown_DEBUG(Keyboard::S)) {
			this->velocity.x -= dx * 800 * deltaTime * 0.7f /*Input::GetStrengthL(0)*/;
			this->velocity.y -= dy * 800 * deltaTime * 0.7f /*Input::GetStrengthL(0)*/;
			if (velocity.y * dy > 0.0f || velocity.x * dx > 0.0f) {
				this->velocity.x = (velocity.x / (1 + (0.9f * deltaTime)));
				this->velocity.y = (velocity.y / (1 + (0.9f * deltaTime)));
			}
		}
		if (!Input::CheckButton(Keys::R_TRIGGER, HELD, 0) && !Input::IsKeyDown_DEBUG(Keyboard::S) && !Input::CheckButton(Keys::L_TRIGGER, HELD, 0) && !Input::IsKeyDown_DEBUG(Keyboard::W)) {
		
			this->velocity.x = (velocity.x / (1 + (0.3f * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (0.3f * deltaTime)));
		}
	}
	//Driving Mode: Directional Smooth
	else if (drivingMode == 2)
	{
		if (Input::GetDirectionL(0) != Vector2())
		{
			Vector2 targetDir(Input::GetDirectionL(0));

			if ((targetDir - this->currentDir).Length() > 0.01f)
			{
				this->currentDir = Vector2::Lerp(this->currentDir, targetDir, deltaTime*2);
				this->currentDir.Normalize();
			}
			else
			{
				this->currentDir = targetDir;
			}

			float newRot = atan2(this->currentDir.x, this->currentDir.y);
			this->vehicle->setRotation(Vector3(0, newRot, 0));

			//v = v0 + a * t
			this->velocitySimple = this->velocitySimple + 1.2f/*accelerationSimple*/ * deltaTime;
		}
		else
		{
			if (this->velocitySimple < 0.05f)
			{
				this->velocitySimple = 0;
			}
			else
			{
				this->velocitySimple /= 1 + (1.0f * deltaTime);
			}
		}
		//OutputDebugStringA((std::to_string(this->velocitySimple) + "\n").c_str());
		Vector3 direction(this->currentDir.x, 0, this->currentDir.y);
		//s = s0 + v * t
		this->vehicle->move(direction* this->velocitySimple* deltaTime);
	}
	//Driving Mode: Old Turn towards direction, semi-realistic
	else if (drivingMode == 3) {
		if (Input::GetStrengthL(0) > 0) {
			if (velocitySpeed < (5000* updatedStats.maxSpeed)) {
				this->velocity.x += dx * deltaTime * 800 * Input::GetStrengthL(0) * updatedStats.accelerationRate;
				this->velocity.y += dy * deltaTime * 800 * Input::GetStrengthL(0) * updatedStats.accelerationRate;
			}

			if (strength < Input::GetStrengthL(0)) {
				strength = Input::GetStrengthL(0);
			}
			else {
				strength -= 0.01f * deltaTime;
			}



			if (vehicleRotation < targetRotation) {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += deltaTime*5;
					if (rotateAcceleration > 30.4f) {
						rotateAcceleration = 30.4f;						
					}
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((((0.076f) + rotateAcceleration * 1) / 2) * DirectX::XM_PI / 180)*deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
				else {
					rotateAcceleration += deltaTime*-5;
					if (rotateAcceleration < -30.4f) {
						rotateAcceleration = -30.4f;						
					}
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((((-0.076f) + rotateAcceleration * 1 ) / 2)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
			}

			else {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += deltaTime*-5;
					if (rotateAcceleration < -30.4f) {
						rotateAcceleration = -30.4f;						
					}
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((((-0.076f) + rotateAcceleration * 1) / 2)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
				else {
					rotateAcceleration += deltaTime*5;
					if (rotateAcceleration > 30.4f) {
						rotateAcceleration = 30.4f;						
					}
					vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((((0.076f) + rotateAcceleration * 1) / 2)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
			}
		}
		else {
			this->velocity.x = (velocity.x / (1 + (3 * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (3 * deltaTime)));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 20;
			//vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
			vehicle->getRigidBody()->setAngularVelocity(btVector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
		}

		rotateAcceleration /= 1.0f + 3*deltaTime;
		/*if (abs(rotateAcceleration) < 0.5) {
			rotateAcceleration = 0;
		}*/
	}


	//Drifting
	float hypoC = sqrt(pow(dx, 2) + (pow(dy, 2)));
	float driftForce = velocity.x * (dy / hypoC) + velocity.y * -(dx / hypoC);
	Vector2 driftResistance = Vector2(-((dy / hypoC) * 2000 * deltaTime) * updatedStats.handlingRate, -(-((dx / hypoC) * 2000 * deltaTime)) * updatedStats.handlingRate);
	if (abs(driftForce) < 250) {
		driftResistance = driftResistance * (abs(driftForce)*0.005f);
	}
	if (drivingMode != 2) {
		if (Input::GetStrengthL(0) > 0) {
			if (driftForce < -0) {
				this->velocity.x -= driftResistance.x;
				this->velocity.y -= driftResistance.y;
			}
			else if (driftForce > 0) {
				this->velocity.x += driftResistance.x;
				this->velocity.y += driftResistance.y;
			}
		}
		else {
			if (driftForce < -0) {
				this->velocity.x -= driftResistance.x;
				this->velocity.y -= driftResistance.y;
			}
			else if (driftForce > 0) {
				this->velocity.x += driftResistance.x;
				this->velocity.y += driftResistance.y;
			}
		}
	}

	//this->vehicle->move(Vector3((velocity.x * deltaTime *0.002f), 0.00f, -(velocity.y * deltaTime * 0.002f)));
	this->vehicle->getRigidBody()->setLinearVelocity(btVector3((velocity.x* deltaTime * 0.002f), 0.00f, -(velocity.y * deltaTime * 0.002f))*100.0f);



	//Body Rotation
	Vector2 curPos3 = Vector2(bodyPivot.x, bodyPivot.z);
	Vector2 position3 = Vector2(vehicle->getPosition().x, vehicle->getPosition().z);

	float dx3 = (curPos3.x - position3.x);
	float dy3 = (curPos3.y - position3.y);

	float hypoC3 = sqrt(pow(dx3, 2) + (pow(dy3, 2)));

	
	

	float springValue = 8.2f;
	Vector2 currentToTarget = Vector2(dx3 / hypoC3, dy3 / hypoC3);
	Vector2 springForce = currentToTarget * springValue;
	Vector2 dampingForce = Vector2(((-accelerator.x)) * 0.2f * sqrt(springValue), ((-accelerator.z)) * 0.2f * sqrt(springValue));
	Vector2 force = springForce + dampingForce;

	/*if (!(dx3 == 0 && hypoC3 == 0) && !(dy3 == 0 && hypoC3 == 0)) {
		this->accelerator.x += force.x * deltaTime * 2.0f;
		this->accelerator.z += force.y * deltaTime * 2.0f;
	}*/

	if ((bodyPivot.x - vehicle->getPosition().x) > 3 && vehicle->getPosition().x < bodyPivot.x) {
		this->accelerator.x += 10 * deltaTime * 2.0f;
	}
	if ((bodyPivot.x - vehicle->getPosition().x) < -3 && vehicle->getPosition().x > bodyPivot.x) {
		this->accelerator.x -= 10 * deltaTime * 2.0f;
	}
	if ((bodyPivot.z - vehicle->getPosition().z) > 3 && vehicle->getPosition().z < bodyPivot.z) {
		this->accelerator.z += 10 * deltaTime * 2.0f;
	}
	if ((bodyPivot.z - vehicle->getPosition().z) < -3 && vehicle->getPosition().z > bodyPivot.z) {
		this->accelerator.z -= 10 * deltaTime * 2.0f;
	}
	
	if (!(dx3 == 0 && hypoC3 == 0) && !(dy3 == 0 && hypoC3 == 0)) {
		this->accelerator.x += force.x * deltaTime * 2.0f;
		this->accelerator.z += force.y * deltaTime * 2.0f;
	}

	Vector3 rotationVec = vehicle->getPosition() - bodyPivot;
	//Vector3 rotationRadians = DirectX::XMVector3AngleBetweenVectors(Vector3(0,1,0), rotationVec);
	//Vector3 rotationRadians = Vector3(DirectX::XMVector3Dot(DirectX::XMVector3Normalize(Vector3(0, 1, 0)), DirectX::XMVector3Normalize(rotationVec)));
	float angle = acos(Vector3(0, 1, 0).Dot(DirectX::XMVector3Normalize(rotationVec)));
	Vector3 axis = Vector3(0, 1, 0).Cross(DirectX::XMVector3Normalize(rotationVec));

	/*float rotx = atan2(y, z)
	float roty = atan2(x * Math.cos(rotx), z)
	float rotz = atan2(Math.cos(rotx), Math.sin(rotx) * Math.sin(roty))*/

	//this->bodyRotation->setPosition(Vector3(vehicle->getPosition().x, vehicle->getPosition().y + 1, vehicle->getPosition().z));
	//this->bodyRotation->move(Vector3(-accelerator.x*deltaTime, 0.00f * 0.0000f, -accelerator.z* deltaTime));
	bodyPivot += Vector3(-accelerator.x * deltaTime, 0.00f * 0.0000f, -accelerator.z * deltaTime);
	//this->bodyRotation->setPosition(Vector3(bodyRotation->getPosition().x, bodyRotation->getPosition().y + 1, bodyRotation->getPosition().z));
	//this->bodyRotation->move(Vector3((velocity.x* deltaTime * 0.002f), 0.00f, -(velocity.y * deltaTime * 0.002f)));
	add += 0.2f * deltaTime;
	//this->bodyRotation->setRotation(-Vector3(rotationVec.x *2, -vehicle->getRotation().y, rotationVec.z * 2));
	//this->bodyRotation->setRotation(-Vector3(0, -vehicle->getRotation().y, 0));
	
	
	//bodyRotationPoint->setPosition(Vector3(bodyPivot.x,bodyPivot.y,bodyPivot.z));
	updateWeapons(deltaTime * 0.5);
}

void Vehicle::updateWeapons(float deltaTime)
{
	this->timeSinceLastShot += deltaTime;

	if (Input::CheckButton(CONFIRM, HELD, 0))
	{
		if (this->timeSinceLastShot >= this->weapon.fireRate)
		{
			this->timeSinceLastShot = fmod(this->timeSinceLastShot, this->weapon.fireRate);
			
			for (int i = 0; i < Vehicle::bulletCount; ++i)
			{
				if (bullets[i].getTimeLeft() == 0.0)
				{
					Vector2 dir = Input::GetDirectionR(0);
					auto playerVelocity = this->vehicle->getRigidBody()->getLinearVelocity();

					this->bullets[i].setWeaponType(this->weapon.type);
					this->bullets[i].shoot(this->vehicle->getPosition() + Vector3(0, 2, 0),
						                   Vector3(dir.x, 0.0, dir.y),
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
	this->health = std::clamp(this->health + amount, 0, this->updatedStats.maxHealth);
	Game::getGraphics().addParticle2(this->vehicle->getPosition(), Vector3(0, 0, 0), 2, 1);
}

bool Vehicle::isDead() const
{
	return this->health <= 0;
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