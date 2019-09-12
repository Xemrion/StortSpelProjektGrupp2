#include "Vehicle.h"


Vehicle::Vehicle()
{
}

void Vehicle::init(Graphics& graphics)
{

	this->vehicle = new GameObject;
	vehicle->mesh = graphics.getMeshPointer("Cube");
	graphics.addToDraw(vehicle);
	vehicle->setPosition(Vector3(0, 0, 0));
	//vehicle->setColor(Vector4(2,0.3,0.3,1));
	vehicle->setScale(Vector3(0.5, 0.5, 1.0));
	graphics.loadTexture("brickwall.tga");
	vehicle->setTexture(graphics.getTexturePointer("brickwall.tga"));


	velocity = Vector2(0,0);
	targetRotation = 0;
	drivingMode = 0;
	topSpeed = 1700;
}

void Vehicle::update(float deltaTime, std::unique_ptr<DirectX::Keyboard> &kb)
{
	auto key = kb->GetState();

	float dx = sin((3.14159265359 / 180) /** vehicle->getRotation()*/);
	float dy = -cos((3.14159265359 / 180)/** vehicle->getRotation()*/);

	if (key.A)
		velocity.x -= 0.01;
	if (key.D)
		velocity.x += 0.01;
	if (key.W)
		velocity.y += 0.01;
	if (key.S)
		velocity.y -= 0.01;
	/*if (key.W) {
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
			this->velocity.x += dx * 7;
			this->velocity.y += dy * 7;

			

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
		}
	}*/

	this->vehicle->move(Vector3(velocity.x * deltaTime , 0.00, velocity.y * deltaTime ));
}
