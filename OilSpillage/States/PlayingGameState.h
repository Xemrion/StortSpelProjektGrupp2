#pragma once 

#include <array>
#include "GameState.h"
#include "../game.h"
#include "../ImGui/imgui.h"
#include "../vehicle.h"
#include "../AI/ActorManager.h"
#include "../Road Network/RoadNet.h"
#include "../DynamicCamera.h"
#include "../PG/Map.hpp"
#include "../UI/UserInterface.h"
#include "../Powerup.h"

#include"..////Objectives/ObjectiveHandler.h"
class PlayingGameState : public GameState {
	friend class Game;
public:
	enum  Menu {
		MENU_PLAYING,
		MENU_PAUSED,
		MENU_OPTIONS,
		MENUCOUNT
	};

	             PlayingGameState();
	virtual     ~PlayingGameState();
	void         update(float delta);
	const float &getTimeRef() const noexcept;
	float        getTime() const noexcept;
	void         setTime(float) noexcept;
	void         changeTime(float delta) noexcept;
	void         setCurrentMenu(Menu);
	Vehicle     *getPlayer() const;
	std::string  getMinimap() const;
	Vector3      getTopLeft() const;
	Vector3      getBottomRight() const;
	ObjectiveHandler& getObjHandler();
	void spawnObjects();
	Vector3 generateObjectivePos(float minDistance, float maxDistance) noexcept;
	Vector3 generateObjectivePos(Vector3 origin, float minDistance, float maxDistance) noexcept;
	PointLight* addLight(PointLight& light);
	void removeLight(PointLight* light);
	SpotLight* addLight(SpotLight& light);
	void removeLight(SpotLight* light);
	LaserLight* addLight(LaserLight& light);
	void removeLight(LaserLight* light);
	void		 moveObjects();
	void		 updateObjects();
	void		 paperCollision(float deltaTime);
	Vector3   getRespawnPosition() const noexcept;
	void addPowerUp(PowerUp p);
	void generateMapPowerUps();
	void clearPowerUps();
	void generateObjectives();

	Vector3 getCameraPos();

	std::unique_ptr<Vehicle>& getPlayer();
	void setPlayer(Vehicle* theVehicle);
	ActorManager* actorManager;

private:
	friend class Game;
   #if defined(_DEBUG) || defined(RELEASE_DEBUG)
	   bool                         pausedTime;
   #endif // _DEBUG
	std::string                     minimap;
	Vector3                         topLeft;
	Vector3                         bottomRight;
	int                             currentMenu;
	bool                            isUsingManhattanDistance { true };
	float                           cameraDistance           { 25   };
	float                           time;
	MapConfig                       config;
	Graphics                       &graphics;
	AStar                          *aStar;
	std::unique_ptr<Map>            map;
	std::unique_ptr<LightList>      lightList;
	std::unique_ptr<Vehicle>        player;
	std::unique_ptr<DynamicCamera>  camera;
	std::unique_ptr<UserInterface>  menues[MENUCOUNT];
	std::unique_ptr<RoadNetwork>    testNetwork;
	std::vector<CinematicPos>       points;
	std::vector<std::unique_ptr<PowerUp>> powerUps;
	SpotLight                      *playerLight;
	GameObject*						testObjective; //Test
	GameObject*		testObjective2; //Test
	GameObject* cameraObject;

	GameObject* objTestPickUp;
	GameObject* objTestPickUp2;
	GameObject* objTestPickUp3;
	GameObject** objArray = new GameObject * [3];
	ObjectiveHandler objectives;
	RNG rng{ RD()() };        // gör privat klassmedlem istället
	int frameCount = 0;
	int nrOfEnemies = 0;

	//Bullet
	std::unique_ptr<Physics>		physics;
	std::unique_ptr<GameObject>		buildingTest;
	Vector<UPtr<GameObject>>		physicsObjects;
	int physicsObjID;
	int count;
	Vector3 prevAccelForce;
	Vector3 accelForce;

	int								spawnTimer = 0;
	float							soundAggro;
	const char* current_item;
	void fillTestParticle();
	const char* items;
	float timer = 0.0f;
	int                             addNrOfParticles  {     1 };
	float                             lifeTime          {     1 };
	float                           timerForParticle  {   .0f };
	float                           vectorFieldPower  {  4.0f };
	float                           vectorFieldSize   {  2.2f };
	float                           randomPosPower    {  0.0f };
	float                           size1             { .039f };
	float                           size2             { .063f };
	float                           colors  [4]       {};
	float                           colors2 [4]       {};
	float                           colors3 [4]       {};
	float                           colors4 [4]       {};
	Vector4                         colorsP [4]       { Vector4( 0.98f, 0.88f, 0.0f, 1.0f ),
	                                                    Vector4( 0.99f,  0.13f, .0f, 1.0f ),
	                                                    Vector4( 0.0f,  0.0f, .0f, 1.0f ),
	                                                    Vector4( 0.0f,  0.0f, .0f, 1.0f )  };
	int electricNew = 0.0f;
	float timerElectric = 0.0f;
	float timerEMP = 0.0f;
	void createElectric(int randNr, float deltaTime);
    void initAI();
	void ImGui_ProcGen();
	void ImGui_Driving();
	void ImGui_Particles();
	void ImGui_Camera();
	void ImGui_AI();
};
