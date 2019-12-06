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

	this->lightList = std::make_unique<LightList>();
	this->lightList->setSun(Sun(Vector3(1.0f, -1.0f, 0.1f), Vector3(1.0f, 0.96f, 0.89f)));
	this->lightList->addLight(PointLight(Vector3(0, 5, 0), Vector3(0.5, 0, 0), 10));
	this->graphics.setLightList(lightList.get());
	this->graphics.setUISun(Vector3(0, 0, 1), Vector4(1, 1, 1, 1));
	graphics.loadShape(SHAPE_CUBE);

	this->camera = std::make_unique<DynamicCamera>(Vector3(0, 0.75f, -3.0f),Vector3(0,0,0));

	this->theVehicle = std::make_unique<Vehicle>();
	graphics.loadModel("Entities/Player", Vector3(3.14f / 2, 0, 0));

	this->physics = std::make_unique<Physics>();
	this->theVehicle->init(physics.get());
	this->theVehicle->setPosition(Vector3(0, 0, 0));
	this->theVehicle->getVehicleBody1()->setPosition(Vector3(0, 0.55, 0));

	Game::getGraphics().loadTexture("UI/garageThing");
	this->textureBG = Game::getGraphics().getTexturePointer("UI/garageThing");
	assert(textureBG && "Could not load texture!");
}

UpgradingGameState::~UpgradingGameState()
{
	
}

Vehicle* UpgradingGameState::getVehicle()
{
	return this->theVehicle.get();
}

void UpgradingGameState::update(float deltaTime)
{
	timer += deltaTime;
	timer = fmod(timer, XM_2PI);
	
	this->theVehicle->updateWeapon(deltaTime);
	this->theVehicle->update(deltaTime, 0, 0, 0, Vector2(0, 0));

	if (Input::getStrengthRnoMouse() > 0.0f)
	{
		Quaternion rotationNow = this->theVehicle->getRotationQuaternion();
		Quaternion rotationDest = Quaternion::Lerp(rotationNow, Quaternion::CreateFromAxisAngle(Vector3(0, 1, 0), std::atan2f(Input::getDirectionRnoMouse().x, Input::getDirectionRnoMouse().y)), deltaTime * 4);

		btQuaternion trams(rotationDest.x, rotationDest.y, rotationDest.z, rotationDest.w);
		this->theVehicle->getRigidBody()->getWorldTransform().setRotation(trams);
	}
	else
	{
		this->theVehicle->getRigidBody()->setAngularVelocity(btVector3(0, deltaTime * 150, 0));
	}
	
	this->theVehicle->setWheelRotation(deltaTime);
	this->physics->update(deltaTime);

	this->graphics.clearScreen(Vector4(Colors::Red));
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	UserInterface::getSpriteBatch()->Draw(this->textureBG->getShaderResView(), Vector2());
	UserInterface::getSpriteBatch()->End();

	this->graphics.render(this->camera.get(), deltaTime);

	this->menues[currentMenu]->update(deltaTime);
	this->graphics.renderUI(deltaTime);

	this->graphics.presentScene();
}

void UpgradingGameState::setCurrentMenu(Menu menu)
{
	this->currentMenu = static_cast<int>(menu);
}

std::unique_ptr<Vehicle>& UpgradingGameState::getPlayer()
{
	return this->theVehicle;
}

void UpgradingGameState::setPlayer(Vehicle* theVehicle)
{
	this->theVehicle.reset(theVehicle);// = std::make_unique<Vehicle>(theVehicle);
}
