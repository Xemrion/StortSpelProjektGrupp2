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
	void addTime(float time);
	void		 spawnObjects();
	Vector3 generateObjectivePos(float minDistance, float maxDistance) noexcept;
private:
	friend class Game;
   #if _DEBUG | RELEASE_DEBUG
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
	ActorManager                   *actorManager;
	std::unique_ptr<LightList>      lightList;
	std::unique_ptr<Vehicle>        player;
	std::unique_ptr<DynamicCamera>  camera;
	std::unique_ptr<UserInterface>  menues[MENUCOUNT];
	std::unique_ptr<RoadNetwork>    testNetwork;
	std::vector<CinematicPos>       points;
	std::vector<PowerUp>		    powerUps;
	SpotLight                      *playerLight;
	GameObject*						testObjective; //Test
	GameObject* objTestPickUp;
	GameObject* objTestPickUp2;
	GameObject* objTestPickUp3;
	GameObject** objArray = new GameObject * [3];
	ObjectiveHandler objectives;
	RNG rng{ RD()() };        // gör privat klassmedlem istället
	
	//Bullet
	std::unique_ptr<Physics>		physics;
	std::unique_ptr<GameObject>		buildingTest;
	Vector<UPtr<GameObject>>		physicsObjects;

	int                             addNrOfParticles  {     2 };
	int                             lifeTime          {     1 };
	float                           timerForParticle  {   .0f };
	float                           vectorFieldPower  {  4.0f };
	float                           vectorFieldSize   {  2.2f };
	float                           randomPosPower    {  0.5f };
	float                           size1             { .069f };
	float                           size2             { .233f };
	float                           colors  [4]       {};
	float                           colors2 [4]       {};
	float                           colors3 [4]       {};
	float                           colors4 [4]       {};
	Vector4                         colorsP [4]       { Vector4( 0.98f, 0.88f, 0.0f, 1.0f ),
	                                                    Vector4( 0.99f,  0.13f, .0f, 1.0f ),
	                                                    Vector4( 0.0f,  0.0f, .0f, 1.0f ),
	                                                    Vector4( 0.0f,  0.0f, .0f, 1.0f )  };
  
    void initAI();
	void ImGui_ProcGen();
	void ImGui_Driving();
	void ImGui_Particles();
	void ImGui_Camera();
	void ImGui_AI();
};
