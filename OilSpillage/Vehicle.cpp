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

	targetRotation = 0;
	drivingMode = 0;
	topSpeed = 4700;
	this->counter = 0.0f;
	this->rotateAcceleration = 0.0f;
	this->rotationSmoother = 1.0f;
	this->tempTargetRotation = 0.0f;
}

Vehicle::~Vehicle()
{
	delete vehicle;
	delete bodyRotation;
}

void Vehicle::init()
{
	this->vehicle = new GameObject;
	vehicle->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(vehicle);
	vehicle->setPosition(Vector3(0.0f, 0.0f, 0.0f));
	//vehicle->setColor(Vector4(2,0.3,0.3,1));
	vehicle->setScale(Vector3(0.75f, 0.2f, 0.8f));
	Game::getGraphics().loadTexture("brickwall.tga");
	vehicle->setTexture(Game::getGraphics().getTexturePointer("brickwall.tga"));

	this->bodyRotation = new GameObject;
	bodyRotation->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(bodyRotation);
	bodyRotation->setPosition(Vector3(0.0f, 0.2f, 0.0f));
	//vehicle->setColor(Vector4(2,0.3,0.3,1));
	bodyRotation->setScale(Vector3(0.5f, 0.12f, 0.9f));
	Game::getGraphics().loadTexture("CarTemp.tga");
	bodyRotation->setTexture(Game::getGraphics().getTexturePointer("CarTemp.tga"));

}

void Vehicle::update(float deltaTime)
{
	tempTargetRotation = targetRotation;


	//Current rotation of vehicle
	float vehicleRotation = fmod((vehicle->getRotation().y * (180 / DirectX::XM_PI)), 360.0f);
	if (vehicleRotation < 0)
		vehicleRotation += 360;
		
	
	//Direction of Vehicle in dx & dy
	float dx = sin((DirectX::XM_PI / 180) * vehicleRotation);
	float dy = -cos((DirectX::XM_PI / 180)* vehicleRotation);


	targetRotation = (atan2(-Input::GetDirectionL(0).x, -Input::GetDirectionL(0).y) * 180 / DirectX::XM_PI) + 180;

	if (Input::IsKeyDown_DEBUG(Keyboard::W)) {
		targetRotation = 0;
	}
	if (Input::IsKeyDown_DEBUG(Keyboard::A)) {
		targetRotation = 270;
	}
	if (Input::IsKeyDown_DEBUG(Keyboard::S)) {
		targetRotation = 180;
	}
	if (Input::IsKeyDown_DEBUG(Keyboard::D)) {
		targetRotation = 90;
	}
	if (Input::IsKeyDown_DEBUG(Keyboard::W) && Input::IsKeyDown_DEBUG(Keyboard::A)) {
		targetRotation = 315;
	}
	if (Input::IsKeyDown_DEBUG(Keyboard::A) && Input::IsKeyDown_DEBUG(Keyboard::S)) {
		targetRotation = 225;
	}
	if (Input::IsKeyDown_DEBUG(Keyboard::S) && Input::IsKeyDown_DEBUG(Keyboard::D)) {
		targetRotation = 135;
	}
	if (Input::IsKeyDown_DEBUG(Keyboard::D) && Input::IsKeyDown_DEBUG(Keyboard::W)) {
		targetRotation = 45;
	}
	//Driving mode: Tryck åt det hållet du vill åka, semi-realistic
	if (drivingMode == 0) {
	
		if (Input::GetStrengthL(0) > 0) {
			this->velocity.x += dx * 0.2 *deltaTime* 4000 * Input::GetStrengthL(0);
			this->velocity.y += dy * 0.2 * deltaTime * 4000 * Input::GetStrengthL(0);

			if (strength < Input::GetStrengthL(0)) {
				strength = Input::GetStrengthL(0);
			}
			else {
				strength -= 0.01 * deltaTime;
			}

			if (velocity.x > topSpeed*strength) {
				velocity.x = topSpeed * strength;
			}
			if (velocity.x < -topSpeed * strength) {
				velocity.x = -topSpeed * strength;
			}
			if (velocity.y > topSpeed * strength) {
				velocity.y = topSpeed * strength;
			}
			if (velocity.y < -topSpeed * strength) {
				velocity.y = -topSpeed * strength;
			}
			if (velocity.x > topSpeed * strength * 0.75f && velocity.y < -topSpeed * strength * 0.75f) {
				velocity.x = topSpeed * strength * 0.75f;
				velocity.y = -topSpeed * strength * 0.75f;
			}
			if (velocity.x < -topSpeed * strength * 0.75f && velocity.y > topSpeed * strength * 0.75f) {
				velocity.x = -topSpeed * strength * 0.75f;
				velocity.y = topSpeed * strength * 0.75f;
			}
			if (velocity.y > topSpeed * strength * 0.75f && velocity.x > topSpeed * strength * 0.75f) {
				velocity.y = topSpeed * strength * 0.75f;
				velocity.x = topSpeed * strength * 0.75f;
			}
			if (velocity.y < -topSpeed * strength * 0.75f && velocity.x < -topSpeed * strength * 0.75f) {
				velocity.y = -topSpeed * strength * 0.75f;
				velocity.x = -topSpeed * strength * 0.75f;
			}


			if (vehicleRotation < targetRotation) {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += 0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration > 1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = 1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3) * DirectX::XM_PI / 180)*deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
				else {
					rotateAcceleration += -0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration < -1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = -1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((-1.9f * 200 * 0.002f) + rotateAcceleration * 2 ) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
			}

			else {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += -0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration < -1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = -1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((-1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
				else {
					rotateAcceleration += 0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration > 1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = 1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 60 * min(((abs(velocity.x) + abs(velocity.y)) / 3000), 1), 0));
				}
			}
		}
		else {
			this->velocity.x = (velocity.x / (1 + (0.001f * 3000 * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (0.001f * 3000 * deltaTime)));

			rotateAcceleration /= 1.0f + 0.005f * deltaTime * 200;
			vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 60 * ((abs(velocity.x) + abs(velocity.y)) / 3000), 0));
		}

		rotateAcceleration /= 1.0f + 0.002f*deltaTime*1500;
	}
	
	
	//if ((tempTargetRotation != targetRotation) && (drivingMode == 0)) {
	//	//rotationSmoother = 1;
	//	rotateAcceleration = 0;
	//}




	//Drifting
	float hypoC = sqrt(pow(dx, 2) + (pow(dy, 2)));
	float driftForce = velocity.x * (dy / hypoC) + velocity.y * -(dx / hypoC);
	if (drivingMode != 2) {
		if (Input::IsKeyDown_DEBUG(Keyboard::W) || Input::IsKeyDown_DEBUG(Keyboard::A) || Input::IsKeyDown_DEBUG(Keyboard::S) || Input::IsKeyDown_DEBUG(Keyboard::D)) {
			if (driftForce < 0) {
				this->velocity.x -= -((dy / hypoC) * 8000 * deltaTime);
				this->velocity.y -= -(-((dx / hypoC) * 8000 * deltaTime));
			}
			else if (driftForce > 0) {
				this->velocity.x += -((dy / hypoC) * 8000 * deltaTime);
				this->velocity.y += -(-((dx / hypoC) * 8000 * deltaTime));
			}
		}
		else {
			if (driftForce < 0) {
				this->velocity.x -= -((dy / hypoC) * 8000 * deltaTime);
				this->velocity.y -= -(-((dx / hypoC) * 8000 * deltaTime));
			}
			else if (driftForce > 0) {
				this->velocity.x += -((dy / hypoC) * 8000 * deltaTime);
				this->velocity.y += -(-((dx / hypoC) * 8000 * deltaTime));
			}
		}
	}


	this->vehicle->move(Vector3((velocity.x * deltaTime *0.002f), 0.00f, -(velocity.y * deltaTime * 0.002f)));




	//Body Rotation
	Vector2 curPos3 = Vector2(bodyRotation->getPosition().x, bodyRotation->getPosition().z);
	Vector2 position3 = Vector2(vehicle->getPosition().x, vehicle->getPosition().z);

	float dx3 = (curPos3.x - position3.x);
	float dy3 = (curPos3.y - position3.y);

	float hypoC3 = sqrt(pow(dx3, 2) + (pow(dy3, 2)));

	
	

	float springValue = 0.02f;
	Vector2 currentToTarget = Vector2(dx3 / hypoC3, dy3 / hypoC3);
	Vector2 springForce = currentToTarget * springValue;
	Vector2 dampingForce = Vector2(-(abs(accelerator.x)) * 2 * sqrt(springValue), -(abs(accelerator.z)) * 2 * sqrt(springValue));
	Vector2 force = springForce + dampingForce;

	if (!(dx3 == 0 && hypoC3 == 0) && !(dy3 == 0 && hypoC3 == 0)) {
		this->accelerator.x += force.x * deltaTime * 2.0f;
		this->accelerator.z += force.y * deltaTime * 2.0f;
	}


	this->bodyRotation->setPosition(Vector3(vehicle->getPosition().x, vehicle->getPosition().y + 1, vehicle->getPosition().z));
	//this->bodyRotation->move(Vector3(-accelerator.x*deltaTime, 0.00f * 0.0000f, -accelerator.z* deltaTime));
	//this->bodyRotation->setPosition(Vector3(bodyRotation->getPosition().x, bodyRotation->getPosition().y + 1, bodyRotation->getPosition().z));
	//this->bodyRotation->move(Vector3((velocity.x* deltaTime * 0.002f), 0.00f, -(velocity.y * deltaTime * 0.002f)));
	this->bodyRotation->setRotation(Vector3(vehicle->getRotation().x, vehicle->getRotation().y , vehicle->getRotation().z + driftForce * 0.0001));
}

float Vehicle::getAcceleratorX()
{
	return accelerator.x;
}
