#pragma once 

#include <array>
#include "GameState.h"
#include "../game.h"
#include "../ImGui/imgui.h"
#include "../vehicle.h"
#include "../AI/Actor.h"
#include "../Road Network/RoadNet.h"
#include "../DynamicCamera.h"
#include "../PG/Map.hpp"
#include "../UI/UserInterface.h"

class PlayingGameState : public GameState {
friend class Game;
public:
	enum  Menu { MENU_PLAYING,
	             MENU_PAUSED,
	             MENU_OPTIONS,
	             MENUCOUNT };

	             PlayingGameState();
	virtual     ~PlayingGameState();
	void         update(float delta);
	const float &getTimeRef() const noexcept;
	float        getTime() const noexcept;
	void         setTime(float) noexcept;
	void         changeTime(float delta) noexcept;
	void         setCurrentMenu(Menu);
	Vehicle     *getPlayer() const;
	std::string	 getMinimap() const;
	Vector3		 getTopLeft() const;
	Vector3		 getBottomRight() const;
	Vector3		 getTileSize() const;
	Vector2		 getTileCount() const;

private:
	friend class Game;
	std::string						minimap;
	Vector3							topLeft;
	Vector3							bottomRight;
	Vector3							tileSize;
	Vector2							tileCount;
	int                             currentMenu;
	bool                            isUsingManhattanDistance { true };
	float                           cameraDistance           { 25   };
	float                           time;
	MapConfig                       config;
	Graphics                       &graphics;
	std::unique_ptr<AStar>          aStar;
	std::unique_ptr<Map>            map;
	std::unique_ptr<Actor>          aiObject;
	std::unique_ptr<LightList>      lightList;
	std::unique_ptr<Vehicle>        player;
	std::unique_ptr<DynamicCamera>  camera;
	std::unique_ptr<UserInterface>  menues[MENUCOUNT];
	std::unique_ptr<RoadNetwork>    testNetwork;
	std::vector<CinematicPos>       points;
	SpotLight                      *playerLight;
	int                             addNrOfParticles  {     2 };
	int                             lifeTime          {     2 };
	float                           timerForParticle  {   .0f };
	float                           vectorFieldPower  {  1.0f };
	float                           vectorFieldSize   {  1.0f };
	float                           randomPosPower    {  1.0f };
	float                           size1             { .020f };
	float                           size2             { .025f };
	float                           colors  [4]       {};
	float                           colors2 [4]       {};
	float                           colors3 [4]       {};
	float                           colors4 [4]       {};
	Vector4                         colorsP [4]       { Vector4( 1.0f, 1.0f, .0f, 1.0f ),
	                                                    Vector4( 1.0f,  .0f, .0f, 1.0f ),
	                                                    Vector4(  .0f,  .0f, .0f, 1.0f ),
	                                                    Vector4(  .1f,  .1f, .1f, 1.0f )  };
  
   void initiateAStar();
	void ImGui_ProcGen();
	void ImGui_Driving();
	void ImGui_Particles();
	void ImGui_Camera();
};
