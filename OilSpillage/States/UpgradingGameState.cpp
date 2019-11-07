#include "UpgradingGameState.h"
#include "../Input.h"

UpgradingGameState::UpgradingGameState() : graphics(Game::getGraphics())
{
	this->timer = 0.0f;
	this->menu = std::make_unique<UIUpgrading>();
	this->menu->init();

	for (int i = 0; i < 20; i++)
	{
		Inventory::instance->addItem(Item::getRandom());
	}
	lightList = std::make_unique<LightList>();
	lightList->setSun(Sun(Vector3(1.0f, -1.0f, 0.1f), Vector3(1.0f, 0.96f, 0.89f)));
	lightList->addLight(PointLight(Vector3(0, 5, 0), Vector3(0.5, 0, 0), 10));
	graphics.setLightList(lightList.get());
	graphics.setUISun(Vector3(0, 0, 1), Vector4(1, 1, 1, 1));

	this->camera = new DynamicCamera(Vector3(0, 4.5f, -7.0f),Vector3(3.14/4,0,0));

	this->theVehicle = new Vehicle;
	graphics.loadModel("Entities/Dummy_Player_Car", Vector3(3.14f / 2, 0, 0));
	physics = std::make_unique<Physics>();
	this->theVehicle->init(physics.get());
	this->theVehicle->getVehicle()->setPosition(Vector3(0, 0, 0));
	this->theVehicle->getVehicleBody1()->setPosition(Vector3(0, 0.55, 0));


	//this->cubeTest->parent = this->cubeTest2;

}

UpgradingGameState::~UpgradingGameState()
{
	delete this->camera;
	delete this->theVehicle;
}

void UpgradingGameState::update(float deltaTime)
{
	timer += deltaTime;
	timer = fmod(timer, 3.14 * 2);
	//this->theVehicle->getVehicleBody1()->setRotation(Vector3(0,timer , 0));
	this->theVehicle->getVehicle()->getRigidBody()->setAngularVelocity(btVector3(0,100*deltaTime, 0));
	this->theVehicle->updateWeapon(deltaTime);
	this->theVehicle->update(deltaTime);
	this->theVehicle->setWheelRotation();
	this->physics->update(deltaTime);



	this->graphics.clearScreen(Vector4(1,1,1,0));
	this->graphics.render(this->camera, deltaTime);

	this->menu->update(deltaTime);
	this->graphics.renderUI(deltaTime);

	this->graphics.presentScene();
}
