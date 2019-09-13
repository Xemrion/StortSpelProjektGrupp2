#include "Vehicle.h"
#include"game.h"

Vehicle::Vehicle()
{
}

void Vehicle::init(Graphics& graphics)
{

	this->vehicle = new GameObject;
	vehicle->mesh = Game::getGraphics().getMeshPointer("Cube");
	Game::getGraphics().addToDraw(vehicle);
	vehicle->setPosition(Vector3(0, 0, 0));
	//vehicle->setColor(Vector4(2,0.3,0.3,1));
	vehicle->setScale(Vector3(0.5, 0.5, 1.0));
	Game::getGraphics().loadTexture("brickwall.tga");
	vehicle->setTexture(Game::getGraphics().getTexturePointer("brickwall.tga"));


	velocity = Vector2(0,0);
	targetRotation = 0;
	drivingMode = 0;
	topSpeed = 4700;
	this->counter = 0.0f;
	this->rotateAcceleration = 0.0f;
	this->rotationSmoother = 1.0f;
	this->tempTargetRotation = 0.0f;
}

void Vehicle::update(float deltaTime, std::unique_ptr<DirectX::Keyboard> &kb)
{
	auto key = kb->GetState();


	tempTargetRotation = targetRotation;


	
	float vehicleRotation = fmod((vehicle->getRotation().y * (180 / 3.14159265359)), 360);
	if (vehicleRotation < 0)
		vehicleRotation += 360;
		
	

	float dx = sin((3.14159265359 / 180) * vehicleRotation);
	float dy = -cos((3.14159265359 / 180)* vehicleRotation);

	/*if (key.A)
		velocity.x -= 0.01;
	if (key.D)
		velocity.x += 0.01;
	if (key.W)
		velocity.y += 0.01;
	if (key.S)
		velocity.y -= 0.01;*/

	if (key.W) {
		targetRotation = 0;
	}

	if (key.A) {
		targetRotation = 270;
	}
	if (key.S) {
		targetRotation = 180;
	}
	if (key.D) {
		targetRotation = 90;
	}
	if (key.W && key.A) {
		targetRotation = 315;
	}
	if (key.A && key.S) {
		targetRotation = 225;
	}
	if (key.S && key.D) {
		targetRotation = 135;
	}
	if (key.D && key.W) {
		targetRotation = 45;
	}
	if (drivingMode == 0) {
	
		if (key.W || key.A || key.S || key.D) {
			this->velocity.x += dx * 2;
			this->velocity.y += dy * 2;

			

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
			if (velocity.x > topSpeed * 0.75 && velocity.y < -topSpeed * 0.75) {
				velocity.x = topSpeed * 0.75;
				velocity.y = -topSpeed * 0.75;
			}
			if (velocity.x < -topSpeed * 0.75 && velocity.y > topSpeed * 0.75) {
				velocity.x = -topSpeed * 0.75;
				velocity.y = topSpeed * 0.75;
			}
			if (velocity.y > topSpeed * 0.75 && velocity.x > topSpeed * 0.75) {
				velocity.y = topSpeed * 0.75;
				velocity.x = topSpeed * 0.75;
			}
			if (velocity.y < -topSpeed * 0.75 && velocity.x < -topSpeed * 0.75) {
				velocity.y = -topSpeed * 0.75;
				velocity.x < -topSpeed * 0.75;
			}


			if (vehicleRotation < targetRotation) {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += 0.01 * 200 * deltaTime * rotationSmoother;
					if (rotateAcceleration > 1.9 * 200 * deltaTime * (rotationSmoother * 2)) {
						rotateAcceleration = 1.9 * 200 * deltaTime * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, (((-1.9 * 200 * deltaTime) + rotateAcceleration * 2) / 3) * 3.14159265359 / 180, 0));
				}
				else {
					rotateAcceleration += -0.01 * 200 * deltaTime * rotationSmoother;
					if (rotateAcceleration < -1.9 * 200 * deltaTime * (rotationSmoother * 2)) {
						rotateAcceleration = -1.9 * 200 * deltaTime * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, (((1.9 * 200 * deltaTime) + rotateAcceleration * 2) / 2) * 3.14159265359 / 180, 0));
				}
			}

			else {
				if (abs(vehicleRotation - targetRotation) < 180) {
					rotateAcceleration += -0.01 * 200 * deltaTime * rotationSmoother;
					if (rotateAcceleration < -2.9 * 200 * deltaTime * (rotationSmoother * 2)) {
						rotateAcceleration = -2.9 * 200 * deltaTime * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, (((-2.9 * 200 * deltaTime) + rotateAcceleration * 2) /3) * 3.14159265359 / 180, 0));
				}
				else {
					rotateAcceleration += 0.01 * 200 * deltaTime * rotationSmoother;
					if (rotateAcceleration > 1.9 * 200 * deltaTime * (rotationSmoother * 2)) {
						rotateAcceleration = 1.9 * 200 * deltaTime * (rotationSmoother * 2);
						
					}
					vehicle->addRotation(Vector3(0, (((1.9 * 200 * deltaTime) + rotateAcceleration * 2) / 3) * 3.14159265359 / 180, 0));
				}
			}
		}
		else {
			this->velocity.x = (velocity.x / (1 + (0.001 * 3000 * deltaTime)));
			this->velocity.y = (velocity.y / (1 + (0.001 * 3000 * deltaTime)));
		}

		rotateAcceleration /= 1.002;
	}
	
	if ((tempTargetRotation != targetRotation) && (drivingMode == 0)) {
		rotationSmoother = 1;
		rotateAcceleration = 0;
	}


	//Drifting
	float hypoC = sqrt(pow(dx, 2) + (pow(dy, 2)));
	float driftForce = velocity.x * (dy / hypoC) + velocity.y * -(dx / hypoC);
	if (drivingMode != 2) {
		if (key.W || key.A || key.S || key.D) {
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


	this->vehicle->move(Vector3((velocity.x * deltaTime *0.002), 0.00, -(velocity.y * deltaTime * 0.002)));
}
