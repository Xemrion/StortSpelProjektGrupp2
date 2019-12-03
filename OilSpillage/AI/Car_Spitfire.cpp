#include "../States/PlayingGameState.h"
#include "Car_Spitfire.h"
#include "../Input.h"


Spitfire::Spitfire(float x, float z, Physics* physics)
	: DynamicActor(x, z,physics)
{
	targetRotation = 0.0f;
	this->rotateAcceleration = 0.0f;
	this->velocitySpeed = 0.0f;
	this->rotationDirection = true;
	this->reverseTimer = 0;
	this->reverseTimer2 = 0;
	this->direction = Vector3(1, 0, 0);
	this->deltaTime = 0;
	throttleInputStrength = 0;
	init(physics);
	setPosition(Vector3(x,- 1.2f, z));
	vehicleBody1->setPosition(Vector3(x, 0 - 1.2f + 0.65f, z));
	this->stats = VehicleStats::AICar;

	float newHealth = stats.maxHealth;
	scaling(newHealth, 1.3);
	setMaxHealth(newHealth);
	setHealth(newHealth);

	this->aggroRange = 500; //TODO: Find better aggro range
	this->setPoints(150 * (1 + (0.1 * Game::getGameInfo().nrOfClearedStages)));
	this->trailTimer = 0.0f;
}

Spitfire::Spitfire()
{
}

Spitfire::~Spitfire()
{
	Game::getGraphics().removeFromDraw(vehicleBody1);
	Game::getGraphics().removeFromDraw(wheel1);
	Game::getGraphics().removeFromDraw(wheel2);
	Game::getGraphics().removeFromDraw(wheel3);
	Game::getGraphics().removeFromDraw(wheel4);
	physics->deletePointJoint(pointJoint);
	delete vehicleBody1;
	delete wheel1;
	delete wheel2;
	delete wheel3;
	delete wheel4;
	if (this->isDead())
	{
		Game::getGameInfo().nrOfCars++;
	}
}

void Spitfire::updateVehicle()
{
	Vector3 prevAccelForce = Vector3(getRigidBody()->getLinearVelocity());
	vehicleMovement(deltaTime, throttleInputStrength, false, false, Vector2(direction.x, direction.z));
	Vector3 accelForce = Vector3(getRigidBody()->getLinearVelocity()) - prevAccelForce;
	setAccelForce(accelForce, deltaTime);
	setWheelRotation();
	this->setPosition(getPosition());

	

}


void Spitfire::move()
{
	direction = destination - this->position;
	direction.Normalize();
	if ((this->position - destination).Length() > 5)
	{
		throttleInputStrength += 0.025 * deltaTime;
		if (throttleInputStrength > 1)
		{
			throttleInputStrength = 1;
		}
	}
}

void Spitfire::update(float dt,const Vector3& targetPos)
{
	DynamicActor::update(dt, targetPos);
	updateVehicle();
	vehicleBody1->setColor(this->getColor());
}

void Spitfire::followPath()
{
	if (pathSize >= 0)
	{
		destination = *path;
		if ((destination - position).Length() < 15)
		{
			path--;
			pathSize--;
		}
	}
	else
	{
		destination = targetPos;
	}
}

void Spitfire::init(Physics* physics)
{
	this->physics = physics;
	mesh = Game::getGraphics().getMeshPointer("Cube");
	setPosition(Vector3(0.0f, 10.0f, 0.0f));
	setScale(Vector3(0.5f, 0.14f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp");
	setTexture(Game::getGraphics().getTexturePointer("CarTemp"));


	this->vehicleBody1 = new GameObject;
	vehicleBody1->mesh = Game::getGraphics().getMeshPointer("Entities/Player1");
	vehicleBody1->setSpotShadow(false);
	Game::getGraphics().addToDraw(vehicleBody1);
	vehicleBody1->setScale(Vector3(0.005f, 0.005f, 0.005f));
	Texture* vehicleTexture = Game::getGraphics().getMaterial("Entities/Player").diffuse;
	vehicleBody1->setTexture(vehicleTexture);

	this->wheel1 = new GameObject;
	this->wheel2 = new GameObject;
	this->wheel3 = new GameObject;
	this->wheel4 = new GameObject;
	wheel1->mesh = Game::getGraphics().getMeshPointer("Entities/Player");
	wheel2->mesh = Game::getGraphics().getMeshPointer("Entities/Player2");
	wheel3->mesh = Game::getGraphics().getMeshPointer("Entities/Player3");
	wheel4->mesh = Game::getGraphics().getMeshPointer("Entities/Player4");
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
	wheel1->setTexture(vehicleTexture);
	wheel2->setTexture(vehicleTexture);
	wheel3->setTexture(vehicleTexture);
	wheel4->setTexture(vehicleTexture);



}

void Spitfire::vehicleMovement(float deltaTime, float throttleInputStrength, bool throttleInputTrigger, bool reverseInputTrigger, Vector2 directionInput)
{

	//Quaternion Rotation to Euler
	btScalar m;
	btScalar n;
	Quaternion qt = Vector4(getRigidBody()->getWorldTransform().getRotation().getX(), getRigidBody()->getWorldTransform().getRotation().getY(), getRigidBody()->getWorldTransform().getRotation().getZ(), getRigidBody()->getWorldTransform().getRotation().getW());

	qt = getRotationQuaternion();

	float heading = getHeading(qt);

	getRigidBody()->getWorldTransform().getRotation().getEulerZYX(n, mRotation, m);

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
	velocitySpeed = (getRigidBody()->getLinearVelocity().getX() * (dx)) + (-getRigidBody()->getLinearVelocity().getZ() * (dy));

	Vector3 steering = Vector3(getRigidBody()->getAngularVelocity().getX(),
		throttleInputStrength * /*deltaTime*/0.035f * 80 * min(velocitySpeed * 0.15f, 1),
		getRigidBody()->getAngularVelocity().getZ());

	Vector3 steering2 = Vector3(getRigidBody()->getAngularVelocity().getX(),
		throttleInputStrength * /*deltaTime*/0.035f * 80 * max(velocitySpeed * 0.15f, -1),
		getRigidBody()->getAngularVelocity().getZ());


	//Driving Mode: Turn towards direction, semi-realistic

	Vector3 steering3 = Vector3(getRigidBody()->getAngularVelocity().getX(),
		/*deltaTime*/0.035f * 80 * min(velocitySpeed * 0.15f, 1),
		getRigidBody()->getAngularVelocity().getZ());

	if (throttleInputStrength > 0 && !this->isDead()) {
		if (velocitySpeed < 0.5f) {
			reverseTimer += 10.0f * deltaTime;
		}
		else {
			reverseTimer = 0;
			reverseTimer2 = 0;
		}
		if ((reverseTimer > 10.0f) && !(reverseTimer2 > 10.0f)) {
			getRigidBody()->applyImpulse(btVector3(-dx * deltaTime * 160.0f * stats.accelerationRate, 0, (dy * deltaTime * 160.0f * stats.accelerationRate)) * throttleInputStrength, btVector3(0, 0, 0));
			reverseTimer2 += 10.0f * deltaTime;
			if (reverseTimer2 > 10.0f) {
				reverseTimer = 0;
				reverseTimer2 = 0;
			}
		}
		else {
			reverseTimer2 = 0;
			if (velocitySpeed < (40 * stats.speed)) {
				getRigidBody()->applyImpulse(btVector3(dx * deltaTime * 160.0f * stats.accelerationRate, 0, -(dy * deltaTime * 160.0f * stats.accelerationRate)) * throttleInputStrength, btVector3(0, 0, 0));
			}
		}

		float difference2 = min((180 - abs(abs(vehicleRotation - targetRotation) - 180)) * 0.05f, 1.0f);

		if (vehicleRotation < targetRotation) {
			if (abs(vehicleRotation - targetRotation) < 180) {
				getRigidBody()->setAngularVelocity(btVector3(0, steering3.y * difference2, 0));
			}
			else {
				getRigidBody()->setAngularVelocity(btVector3(0, -steering3.y * difference2, 0));
			}
		}
		else {
			if (abs(vehicleRotation - targetRotation) < 180) {
				getRigidBody()->setAngularVelocity(btVector3(0, -steering3.y * difference2, 0));
			}
			else {
				getRigidBody()->setAngularVelocity(btVector3(0, steering3.y * difference2, 0));
			}
		}
	}
	else if (!isDead()) {
		getRigidBody()->setLinearVelocity(btVector3(getRigidBody()->getLinearVelocity().getX() / (1 + (1.8f * deltaTime)), getRigidBody()->getLinearVelocity().getY(), getRigidBody()->getLinearVelocity().getZ() / (1 + (1.8f * deltaTime))));

		rotateAcceleration /= 1.0f + 0.005f * deltaTime * 40;
		//vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime*2 * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
		getRigidBody()->setAngularVelocity(btVector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180) * deltaTime * 120 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
	}
	else {
		getRigidBody()->setLinearVelocity(btVector3(getRigidBody()->getLinearVelocity().getX() / (1 + (0.6f * deltaTime)), getRigidBody()->getLinearVelocity().getY(), getRigidBody()->getLinearVelocity().getZ() / (1 + (0.6f * deltaTime))));
		rotateAcceleration /= 1.0f + 0.005f * deltaTime * 40;

		getRigidBody()->setAngularVelocity(btVector3(steering.x, (getRigidBody()->getAngularVelocity().getY() / (1 + (0.8f * deltaTime)))/** min(abs(velocitySpeed), 1)*/, steering.z));
	}

	rotateAcceleration /= 1.0f + 6 * deltaTime;


	//Drifting
	float hypoC = sqrt(pow(dx, 2) + (pow(dy, 2)));
	float driftForce = getRigidBody()->getLinearVelocity().getX() * (dy / hypoC) + -getRigidBody()->getLinearVelocity().getZ() * -(dx / hypoC);
	Vector2 driftResistance = Vector2(-((dy / hypoC) * 4000 * deltaTime) * stats.handlingRate, -(-((dx / hypoC) * 4000 * deltaTime)) * stats.handlingRate);
	if (abs(driftForce) < 250) {
		driftResistance = driftResistance * (abs(driftForce) * 0.005f);
	}
	if (Input::getStrengthL() > 0) {
		if (driftForce < -0) {
			getRigidBody()->applyImpulse(btVector3(-driftResistance.x, 0, 0), btVector3(0, 0, 0));
			getRigidBody()->applyImpulse(btVector3(0, 0, driftResistance.y), btVector3(0, 0, 0));
		}
		else if (driftForce > 0) {
			getRigidBody()->applyImpulse(btVector3(driftResistance.x, 0, 0), btVector3(0, 0, 0));
			getRigidBody()->applyImpulse(btVector3(0, 0, -driftResistance.y), btVector3(0, 0, 0));
		}
	}
	else {
		if (driftForce < -0) {
			getRigidBody()->applyImpulse(btVector3(-driftResistance.x, 0, 0), btVector3(0, 0, 0));
			getRigidBody()->applyImpulse(btVector3(0, 0, driftResistance.y), btVector3(0, 0, 0));
		}
		else if (driftForce > 0) {
			getRigidBody()->applyImpulse(btVector3(driftResistance.x, 0, 0), btVector3(0, 0, 0));
			getRigidBody()->applyImpulse(btVector3(0, 0, -driftResistance.y), btVector3(0, 0, 0));
		}
	}
	//this->vehicle->getRigidBody()->setLinearVelocity(btVector3((velocity.x * deltaTime*2 * 0.002f) * 100.0f, this->vehicle->getRigidBody()->getLinearVelocity().getY(), -(velocity.y * deltaTime*2 * 0.002f) * 100.0f));
	getRigidBody()->setAngularVelocity(btVector3(0, getRigidBody()->getAngularVelocity().getY(), 0));


	if (deltaTime > 0.01f) {
		soundTimer += 100.0f * deltaTime;
	}
	else {
		soundTimer += 100.0f;
	}

	this->trailTimer += deltaTime;

	Vector3 frontTempDir = Vector3(cos(this->vehicleBody1->getRotation().y - 3.14 / 2), 0, -sin(this->vehicleBody1->getRotation().y - 3.14 / 2));
	Vector3 rightDir = frontTempDir.Cross(Vector3(0.0f, 1.0f, 0.0f));
	rightDir.Normalize();
	Vector3 initialDir = -Vector3(getRigidBody()->getLinearVelocity());
	initialDir.Normalize();
	//Game::getGraphics().addTrail(Vector3(0.0f, -0.5f, 0.0f) - this->vehicleBody1->getPosition() + rightDir*0.5f, -frontTempDir, 1, 20.0f);
	//Game::getGraphics().addTrail(Vector3(0.0f, -0.5f, 0.0f) - this->vehicleBody1->getPosition() - rightDir * 0.5f, -frontTempDir, 1, 20.0f);
	if (this->trailTimer > 0.01f && abs(driftForce) > 5.0f)
	{
		/*Game::getGraphics().addTrail(Vector3(0.0f, -0.6f, 0.0f) - frontTempDir * 0.01f + this->vehicleBody1->getPosition() + rightDir * 0.5f, Vector4(initialDir.x, initialDir.y, initialDir.z, 0.1f * abs(driftForce)), 1, 5.0f);
		Game::getGraphics().addTrail(Vector3(0.0f, -0.6f, 0.0f) - frontTempDir * 0.01f + this->vehicleBody1->getPosition() - rightDir * 0.5f, Vector4(initialDir.x, initialDir.y, initialDir.z, 0.1f * abs(driftForce)), 1, 5.0f);

		Game::getGraphics().addTrail(Vector3(0.0f, -0.6f, 0.0f) + frontTempDir * 1.0f + this->vehicleBody1->getPosition() + rightDir * 0.5f, Vector4(initialDir.x, initialDir.y, initialDir.z, 0.1f * abs(driftForce)), 1, 5.0f);
		Game::getGraphics().addTrail(Vector3(0.0f, -0.6f, 0.0f) + frontTempDir * 1.0f + this->vehicleBody1->getPosition() - rightDir * 0.5f, Vector4(initialDir.x, initialDir.y, initialDir.z, 0.1f * abs(driftForce)), 1, 5.0f);*/
		this->trailTimer = 0;
	}

}

void Spitfire::setAccelForce(Vector3 accelForce, float deltaTime)
{

	//AccelForce
	if ((max(abs(accelForce.x), abs(accelForce.z)) > 5.0f)) {
		int randomSound = rand() % 3 + 1;
		std::string soundEffect = "data/sound/CarImpact" + std::to_string(randomSound) + ".wav";
		int randomSound2 = rand() % 3 + 1;
		std::string soundEffect2 = "data/sound/MetalImpactPitched" + std::to_string(randomSound) + ".wav";
		if (max(abs(accelForce.x), abs(accelForce.z)) > 25.0f) {
			Game::getGraphics().addParticle2(getPosition(), Vector3(0, 0, 0), 2, 1);
			changeHealth(-20.0f);
			Sound::play("data/sound/CarCrash.wav");
			Sound::play(soundEffect2);
		}
		else if (max(abs(accelForce.x), abs(accelForce.z)) > 15.0f) {
			Game::getGraphics().addParticle2(getPosition(), Vector3(0, 0, 0), 2, 1);
			changeHealth(-10.0f);
			Sound::play(soundEffect);
			Sound::play(soundEffect2);
		}
		else {
			Sound::play("data/sound/CarImpactSoft.wav");
			Sound::play(soundEffect2);
		}
	}
	/*else {
		vehicleBody1->setColor(Vector4(vehicleBody1->getColor().x/(1 + 20.0f*deltaTime), vehicleBody1->getColor().y / (1 + 20.0f * deltaTime), vehicleBody1->getColor().z / (1 + 20.0f * deltaTime), 1));
	}*/
}

void Spitfire::setWheelRotation()
{
	wheel1->setPosition(getPosition() + Vector3(0, 0.25f, 0));
	wheel2->setPosition(getPosition() + Vector3(0, 0.25f, 0));
	wheel3->setPosition(getPosition() + Vector3(0, 0.25f, 0));
	wheel4->setPosition(getPosition() + Vector3(0, 0.25f, 0));

	wheel1->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
	wheel2->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
	wheel3->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
	wheel4->setRotation(Vector3(0, vehicleBody1->getRotation().y, 0));
}

float Spitfire::getYaw(DirectX::XMVECTOR Quaternion)
{
	return asin(-2 * (Quaternion.m128_f32[0] * Quaternion.m128_f32[2] - Quaternion.m128_f32[3] * Quaternion.m128_f32[1]));
}

float Spitfire::getHeading(Quaternion qt)
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