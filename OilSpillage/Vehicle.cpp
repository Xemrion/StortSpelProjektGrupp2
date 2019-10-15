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
	this->bodyRotation = nullptr;
	this->bodyRotationPoint = nullptr;

	this->leftoverTime = 0.0f;
	this->weapon = VehicleWeapon::missileLauncher;
	this->defaultStats = VehicleStats::fastCar;
	this->updatedStats = this->defaultStats;

	this->health = this->updatedStats.maxHealth;
}

Vehicle::~Vehicle()
{
	for (int i = 0; i < Vehicle::bulletCount; i++)
	{
		delete this->bullets[i].obj;
	}

	delete vehicle;
	delete bodyRotation;
	delete bodyRotationPoint;
}

void Vehicle::init()
{
	this->vehicle = new GameObject;
	vehicle->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(vehicle);
	vehicle->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	//vehicle->setColor(Vector4(2,0.3,0.3,1));
	vehicle->setScale(Vector3(0.75f, 0.2f, 0.8f));
	Game::getGraphics().loadTexture("brickwall");
	vehicle->setTexture(Game::getGraphics().getTexturePointer("brickwall"));

	this->bodyRotation = new GameObject;
	bodyRotation->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(bodyRotation);
	bodyRotation->setPosition(Vector3(0.0f, 0.2f, 0.0f));
	//vehicle->setColor(Vector4(2,0.3,0.3,1));
	bodyRotation->setScale(Vector3(0.5f, 0.12f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp");
	bodyRotation->setTexture(Game::getGraphics().getTexturePointer("CarTemp"));

	this->bodyRotationPoint = new GameObject;
	bodyRotationPoint->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(bodyRotationPoint);
	bodyRotationPoint->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	//vehicle->setColor(Vector4(2,0.3,0.3,1));
	bodyRotationPoint->setScale(Vector3(0.2f, 0.2f, 0.2f));
	Game::getGraphics().loadTexture("brickwall");
	bodyRotationPoint->setTexture(Game::getGraphics().getTexturePointer("brickwall"));

	bodyPivot = Vector3(0.0f, 1.2f, 0.0f);

	for (int i = 0; i < 16; i++)
	{
		this->bullets[i].obj = new GameObject;
		this->bullets[i].obj->mesh = Game::getGraphics().getMeshPointer("Cube");
		this->bullets[i].obj->setScale(Vector3(0.25f, 0.25f, 0.25f));
		this->bullets[i].obj->setColor(Vector4(1, 1, 0, 1));
	}
}

void Vehicle::update(float deltaTime)
{
	deltaTime *= 2;
	if (Input::CheckButton(CONFIRM, HELD, 0))
	{
		float tempDelta = deltaTime + this->leftoverTime;

		if (tempDelta <= this->weapon.fireSpeed)
		{
			this->leftoverTime = tempDelta;
		}

		while (tempDelta > this->weapon.fireSpeed)
		{
			tempDelta -= this->weapon.fireSpeed;
			int freeToUse = 0;

			while (freeToUse < Vehicle::bulletCount && this->bullets[freeToUse].timeLeft > 0.0f)
			{
				freeToUse++;
			}

			if (freeToUse < Vehicle::bulletCount)
			{
				Vector2 dir = Input::GetDirectionR(0);
				this->bullets[freeToUse].dir = Vector3(dir.x, 0, dir.y);
				this->bullets[freeToUse].dir.Normalize();
				this->bullets[freeToUse].timeLeft = this->weapon.bulletLifetime;
				this->bullets[freeToUse].speed = this->weapon.bulletSpeed + max(abs(velocity.x) , abs(velocity.y));
				this->bullets[freeToUse].obj->setPosition(this->vehicle->getPosition() + Vector3(0, 2, 0));
				this->bullets[freeToUse].obj->setRotation(Vector3(XMVector3AngleBetweenVectors(Vector3(0, 0, 1), this->bullets[freeToUse].dir)) * Vector3(0, 1, 0));
			}
			else
			{
				this->leftoverTime = 0.0f;
				break;
			}

			if (tempDelta <= this->weapon.fireSpeed)
			{
				this->leftoverTime = tempDelta;
			}
		}
	}
	else
	{
		this->leftoverTime = 0.0f;
	}

	for (int i = 0; i < Vehicle::bulletCount; i++)
	{
		Game::getGraphics().removeFromDraw(this->bullets[i].obj);

		if (this->bullets[i].timeLeft > 0.0f)
		{
			this->bullets[i].timeLeft -= deltaTime;
			this->bullets[i].obj->move(this->bullets[i].dir * this->bullets[i].speed * deltaTime);
			Game::getGraphics().addToDraw(this->bullets[i].obj);
		}
	}

	tempTargetRotation = targetRotation;


	//Current rotation of vehicle
	float vehicleRotation = fmod((vehicle->getRotation().y * (180 / DirectX::XM_PI)), 360.0f);
	if (vehicleRotation < 0)
		vehicleRotation += 360;
		
	
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
			this->vehicle->setRotation(Vector3(0, newRot, 0));

		}
		else {
			this->velocity.x = (velocity.x / (1 + (0.001f * 3000 * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (0.001f * 3000 * deltaTime)));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 20;
			vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
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
				vehicle->addRotation(Vector3(0, Input::GetStrengthL(0)* ((((0.19f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
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
				vehicle->addRotation(Vector3(0, Input::GetStrengthL(0)* ((((-0.19f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
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
				vehicle->addRotation(Vector3(0, Input::GetStrengthL(0)* ((((-0.19f) + rotateAcceleration * 2) / 3) * DirectX::XM_PI / 180) * deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
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
				vehicle->addRotation(Vector3(0, Input::GetStrengthL(0)* ((((0.19f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				rotationDirection = true;
			}
		}
		else {
			//rotateAcceleration = 0;
			rotateAcceleration /= 1.0f + 10 * deltaTime;
			vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 8000), 0));

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
					vehicle->addRotation(Vector3(0, ((((0.076f) + rotateAcceleration * 1) / 2) * DirectX::XM_PI / 180)*deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
				else {
					rotateAcceleration += deltaTime*-5;
					if (rotateAcceleration < -30.4f) {
						rotateAcceleration = -30.4f;						
					}
					vehicle->addRotation(Vector3(0, ((((-0.076f) + rotateAcceleration * 1 ) / 2)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
			}

			else {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += deltaTime*-5;
					if (rotateAcceleration < -30.4f) {
						rotateAcceleration = -30.4f;						
					}
					vehicle->addRotation(Vector3(0, ((((-0.076f) + rotateAcceleration * 1) / 2)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
				else {
					rotateAcceleration += deltaTime*5;
					if (rotateAcceleration > 30.4f) {
						rotateAcceleration = 30.4f;						
					}
					vehicle->addRotation(Vector3(0, ((((0.076f) + rotateAcceleration * 1) / 2)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
			}
		}
		else {
			this->velocity.x = (velocity.x / (1 + (3 * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (3 * deltaTime)));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 20;
			vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
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

	this->vehicle->move(Vector3((velocity.x * deltaTime *0.002f), 0.00f, -(velocity.y * deltaTime * 0.002f)));




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

	this->bodyRotation->setPosition(Vector3(vehicle->getPosition().x, vehicle->getPosition().y + 1, vehicle->getPosition().z));
	//this->bodyRotation->move(Vector3(-accelerator.x*deltaTime, 0.00f * 0.0000f, -accelerator.z* deltaTime));
	bodyPivot += Vector3(-accelerator.x * deltaTime, 0.00f * 0.0000f, -accelerator.z * deltaTime);
	//this->bodyRotation->setPosition(Vector3(bodyRotation->getPosition().x, bodyRotation->getPosition().y + 1, bodyRotation->getPosition().z));
	//this->bodyRotation->move(Vector3((velocity.x* deltaTime * 0.002f), 0.00f, -(velocity.y * deltaTime * 0.002f)));
	add += 0.2f * deltaTime;
	//this->bodyRotation->setRotation(-Vector3(rotationVec.x *2, -vehicle->getRotation().y, rotationVec.z * 2));
	this->bodyRotation->setRotation(-Vector3(0, -vehicle->getRotation().y, 0));
	
	

	//bodyRotationPoint->setPosition(Vector3(bodyPivot.x,bodyPivot.y,bodyPivot.z));
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
}

bool Vehicle::isDead() const
{
	return this->health <= 0;
}
