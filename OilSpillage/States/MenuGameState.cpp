#include "MenuGameState.h"
#include "../UI/Menu/UIMainMenu.h"
#include "../UI/Menu/UIOptions.h"
#include "../UI/Menu/UICredits.h"
#include "../UI/Menu/UIControls.h"
#include "../UI/Menu/UIHighscore.h"
#include "../UI/Menu/UILoad.h"
#include <cassert>

MenuGameState::MenuGameState() : graphics(Game::getGraphics()), currentMenu(MENU_MAIN)
{
	this->menues[MENU_MAIN] = std::make_unique<UIMainMenu>();
	this->menues[MENU_MAIN]->init();
	this->menues[MENU_OPTIONS] = std::make_unique<UIOptions>();
	this->menues[MENU_OPTIONS]->init();
	this->menues[MENU_CREDITS] = std::make_unique<UICredits>();
	this->menues[MENU_CREDITS]->init();
	this->menues[MENU_CONTROLS] = std::make_unique<UIControls>();
	this->menues[MENU_HIGHSCORE] = std::make_unique<UIHighscore>();
	this->menues[MENU_LOAD] = std::make_unique<UILoad>();

	Game::getGraphics().loadTexture("UI/image2");
	this->textureLogo = Game::getGraphics().getTexturePointer("UI/image2");
	assert(textureLogo && "Could not load texture!");
	this->slots = std::make_unique<VehicleSlots>();
	this->theVehicle = std::make_unique<Vehicle>();
	graphics.loadModel("Entities/Player", Vector3(3.14f / 2, 0, 0));
	this->physics = std::make_unique<Physics>();
	this->lightList = std::make_unique<LightList>();
	this->lightList->setSun(Sun(Vector3(1.0f, -1.0f, 0.1f), Vector3(1.0f, 0.96f, 0.89f)));
	this->lightList->addLight(PointLight(Vector3(0,-0.25, -0.25), Vector3(0.4, 0.1, 0), 10));

	this->graphics.setLightList(lightList.get());

	this->theVehicle->init(physics.get());
	this->theVehicle->setPosition(Vector3(3.15, -0.25, 0));
	this->theVehicle->getVehicleBody1()->setPosition(Vector3(3.15, 0.45, 0));
	
	this->theVehicle->setRotation(Vector3(0, 2.65, 0));

	this->camera = std::make_unique<DynamicCamera>(Vector3(2.0f, 0.5f, -2.0f), Vector3(0, 0, 0));
	barrels = std::make_unique<GameObject>();
	graphics.loadModel("Entities/Barrel");
	barrels.get()->mesh = graphics.getMeshPointer("Entities/Barrel");
	barrels.get()->setTexture(graphics.getMaterial("Entities/Barrel").diffuse);
	barrels.get()->setPosition(Vector3(0.1, -0.55f, 0));
	barrels.get()->setScale(Vector3(0.4f,0.4f,0.4f));
	
	graphics.getParticleSystem("fire")->setGravity(-0.3f);
	
	graphics.getParticleSystem("fire")->setSize(0.03f,0.02f);
	graphics.getParticleSystem("fire")->changeVectorField(0.75f,0.09f);
	graphics.createFrustumBuffer(camera.get());


	graphics.addToDraw(barrels.get());

}

MenuGameState::~MenuGameState() {

}

void MenuGameState::update(float deltaTime)
{
	this->theVehicle->update(deltaTime, 0, 0, 0, Vector2(0, 0));
	this->theVehicle->setWheelRotation(deltaTime);
	this->physics->update(deltaTime);

	graphics.addParticle("fire", 1, 1.0f, Vector3(0, 0, 0), Vector4(0, 1, 0, 0.5f), 0.25f);
	graphics.addParticle("fire", 1, 1.0f, Vector3(0.1f, 0, 0), Vector4(0, 1, 0, 0.5f), 0.25f);
	graphics.addParticle("fire", 1, 1.5f, Vector3(0, 0, 0.1f), Vector4(0, 1, 0, 0.5f), 0.25f);
	
	this->graphics.clearScreen(Vector4(0,0,0,0));
	this->graphics.render(this->camera.get(), deltaTime);
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	UserInterface::getSpriteBatch()->Draw(this->textureLogo->getShaderResView(), Vector2(SCREEN_WIDTH / 2 - textureLogo->getWidth() / 2, textureLogo->getHeight() - 170));
	UserInterface::getSpriteBatch()->End();

	this->menues[this->currentMenu]->update(deltaTime);

	if (this->oldMenu == MENU_CONTROLS)
	{
		this->menues[MENU_CONTROLS] = std::make_unique<UIControls>();
	}

	this->graphics.presentScene();
}

void MenuGameState::setCurrentMenu(Menu menu)
{
	this->oldMenu = this->currentMenu;
	this->currentMenu = static_cast<int>(menu);

	if (menu == MENU_HIGHSCORE)
	{
		static_cast<UIHighscore*>(this->menues[MENU_HIGHSCORE].get())->init(false, "");
	}
	else if (menu == MENU_LOAD || menu == MENU_CONTROLS)
	{
		this->menues[menu]->init();
	}
}

VehicleSlots* MenuGameState::getSlots()
{
	return this->slots.get();
}

void MenuGameState::unloadTextures()
{
	Game::getGraphics().unloadTexture("UI/image2");
	this->menues[MENU_MAIN]->unloadTextures();
	this->menues[MENU_OPTIONS]->unloadTextures();
	this->menues[MENU_CREDITS]->unloadTextures();
	this->menues[MENU_CONTROLS]->unloadTextures();
	this->menues[MENU_HIGHSCORE]->unloadTextures();
	this->menues[MENU_LOAD]->unloadTextures();
}
