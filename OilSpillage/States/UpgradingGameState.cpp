#include "UpgradingGameState.h"
#include "../Input.h"
#include "../UI/Upgrading/UIUpgrading.h"
#include "../UI/Upgrading/UIRandomItem.h"

UpgradingGameState::UpgradingGameState() : graphics(Game::getGraphics()), currentMenu(MENU_RANDOMITEM)
{
	this->timer = 0.0f;
	this->menues[MENU_UPGRADING] = std::make_unique<UIUpgrading>();
	this->menues[MENU_UPGRADING]->init();
	this->menues[MENU_RANDOMITEM] = std::make_unique<UIRandomItem>();
	this->menues[MENU_RANDOMITEM]->init();

	for (int i = 0; i < 20; i++)
	{
		Inventory::instance->addItem(Item::getRandom());
	}

	this->lightList = std::make_unique<LightList>();
	this->lightList->setSun(Sun(Vector3(1.0f, -1.0f, 0.1f), Vector3(1.0f, 0.96f, 0.89f)));
	this->lightList->addLight(PointLight(Vector3(0, 5, 0), Vector3(0.5, 0, 0), 10));
	this->graphics.setLightList(lightList.get());
	this->graphics.setUISun(Vector3(0, 0, 1), Vector4(1, 1, 1, 1));

	this->camera = std::make_unique<DynamicCamera>(Vector3(0, 4.5f, -5.0f),Vector3(3.14/4,0,0));

	this->theVehicle = std::make_unique<Vehicle>();
	this->graphics.loadModel("Entities/Dummy_Player_Car", Vector3(3.14f / 2, 0, 0));
	this->physics = std::make_unique<Physics>();
	this->theVehicle->init(physics.get());
	this->theVehicle->getVehicle()->setPosition(Vector3(0, 0, 0));
	this->theVehicle->getVehicleBody1()->setPosition(Vector3(0, 0.55, 0));
}

UpgradingGameState::~UpgradingGameState()
{
}

void UpgradingGameState::update(float deltaTime)
{
	timer += deltaTime;
	timer = fmod(timer, XM_2PI);
	
	this->theVehicle->getVehicle()->getRigidBody()->setAngularVelocity(btVector3(0,100*deltaTime, 0));
	this->theVehicle->updateWeapon(deltaTime);
	this->theVehicle->update(deltaTime);
	this->theVehicle->setWheelRotation();
	this->physics->update(deltaTime);

	this->graphics.clearScreen(Vector4(1,1,1,0));
	this->graphics.render(this->camera.get(), deltaTime);

	this->menues[currentMenu]->update(deltaTime);
	this->graphics.renderUI(deltaTime);

	this->graphics.presentScene();
}

void UpgradingGameState::setCurrentMenu(Menu menu)
{
	this->currentMenu = static_cast<int>(menu);
}
