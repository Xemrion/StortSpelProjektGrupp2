#include "Vehicle.h"
#include"game.h"
#include "Input.h"

Vehicle::Vehicle()
{
	velocity = Vector2(0.0f, 0.0f);
	accelerator = Vector3(0.0f, 0.0f, 0.0f);
	acceleratorTempX = 0;
	acceleratorTempZ = 0;

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
	vehicle->setScale(Vector3(0.75f, 0.1f, 1.0f));
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
	
		if (Input::IsKeyDown_DEBUG(Keyboard::W) || Input::IsKeyDown_DEBUG(Keyboard::A) || Input::IsKeyDown_DEBUG(Keyboard::S) || Input::IsKeyDown_DEBUG(Keyboard::D)) {
			this->velocity.x += dx * 2 *deltaTime* 4000;
			this->velocity.y += dy * 2 * deltaTime * 4000;

			

			if (velocity.x > topSpeed) {
				velocity.x = topSpeed;
			}
			if (velocity.x < -topSpeed) {
				velocity.x = -topSpeed;
			}
			if (velocity.y > topSpeed) {
				velocity.y = topSpeed;
			}
			if (velocity.y < -topSpeed) {
				velocity.y = -topSpeed;
			}
			if (velocity.x > topSpeed * 0.75f && velocity.y < -topSpeed * 0.75f) {
				velocity.x = topSpeed * 0.75f;
				velocity.y = -topSpeed * 0.75f;
			}
			if (velocity.x < -topSpeed * 0.75f && velocity.y > topSpeed * 0.75f) {
				velocity.x = -topSpeed * 0.75f;
				velocity.y = topSpeed * 0.75f;
			}
			if (velocity.y > topSpeed * 0.75f && velocity.x > topSpeed * 0.75f) {
				velocity.y = topSpeed * 0.75f;
				velocity.x = topSpeed * 0.75f;
			}
			if (velocity.y < -topSpeed * 0.75f && velocity.x < -topSpeed * 0.75f) {
				velocity.y = -topSpeed * 0.75f;
				velocity.x = -topSpeed * 0.75f;
			}


			if (vehicleRotation < targetRotation) {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += 0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration > 1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = 1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3) * DirectX::XM_PI / 180)*deltaTime * 200, 0));
				}
				else {
					rotateAcceleration += -0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration < -1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = -1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((-1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 200, 0));
				}
			}

			else {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += -0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration < -1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = -1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((-1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 200, 0));
				}
				else {
					rotateAcceleration += 0.01f * 200 * deltaTime*5 * rotationSmoother;
					if (rotateAcceleration > 1.9f * 200 * 0.04f * (rotationSmoother * 2)) {
						rotateAcceleration = 1.9f * 200 * 0.04f * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, ((((1.9f * 200 * 0.002f) + rotateAcceleration * 2) / 3)* DirectX::XM_PI / 180)* deltaTime * 200, 0));
				}
			}
		}
		else {
			this->velocity.x = (velocity.x / (1 + (0.001f * 3000 * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (0.001f * 3000 * deltaTime)));

			rotateAcceleration /= 1.0f + 0.05f * deltaTime * 200;
			vehicle->addRotation(Vector3(0, ((rotateAcceleration)* DirectX::XM_PI / 180)* deltaTime * 200, 0));
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


	
	//bodyRotation
	if (bodyRotation->getPosition().x < vehicle->getPosition().x) {
		accelerator.x += 0.01f * deltaTime;
	}
	else {
		accelerator.x -= 0.01f * deltaTime;
	}
	if (bodyRotation->getPosition().z < vehicle->getPosition().z) {
		accelerator.z += 0.01f * deltaTime;
	}
	else {
		accelerator.z -= 0.01f * deltaTime;
	}
	accelerator.x /= (1 + (0.001f * 3000 * deltaTime));
	accelerator.z /= (1 + (0.001f * 3000 * deltaTime));

	float dx2 = cos((DirectX::XM_PI / 180) * vehicleRotation);
	float dy2 = sin((DirectX::XM_PI / 180) * vehicleRotation);

	float hypoC2 = sqrt(pow(dx2, 2) + (pow(dy2, 2)));
	float accelForce2 = velocity.x * (dy2 / hypoC2) + velocity.y * -(dx2 / hypoC2);

	//this->bodyRotation->move(Vector3(accelerator.x*deltaTime*200, 0.00f, accelerator.z * deltaTime*200));
	this->bodyRotation->move(Vector3((velocity.x* deltaTime * 0.002f), 0.00f, -(velocity.y * deltaTime * 0.002f)));
	//this->bodyRotation->setPosition(Vector3(accelerator.x, accelerator.y+1, accelerator.z));
	this->bodyRotation->setRotation(Vector3(vehicle->getRotation().x, vehicle->getRotation().y , vehicle->getRotation().z + driftForce * 0.0001));
}

float Vehicle::getAcceleratorX()
{
	return accelerator.x;
}
