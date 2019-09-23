#include "game.h"

void Game::addQuad(int x)
{
	GameObject* object2 = new GameObject;
	object2->mesh = graphics.getMeshPointer("sda");
	graphics.addToDraw(object2);
	object2->setPosition(Vector3(x, 0, 0));
	object2->setTexture(graphics.getTexturePointer("fireball.tga"));
}

void Game::init(Window* window)
{
	this->window = window; 
	graphics.init(window, 90);
	graphics.loadMesh("sda");
	graphics.loadShape(SHAPE_CUBE);
	graphics.loadTexture("fireball.tga");
	this->testObject = new GameObject();
	this->testObject->mesh = graphics.getMeshPointer("Cube");
	this->testObject->setTexture(graphics.getTexturePointer("fireball.tga"));
	this->testObject->setPosition(Vector3(-17, -10, 5));

	graphics.addToDraw(testObject);

	this->testNetwork = new RoadNetwork(8473552, Vector2(64.0f, 64.0f), Vector2(0.0f, 0.0f));
	this->testNetwork->generateInitialSegments("FFFFFFFFFFFFH-F-FFFHF+FFFF-FFH");
	this->testNetwork->generateAdditionalSegments("FH+H+F-H-H-FFF", 5);
	this->testNetwork->saveTestNetwork("test.txt");
	this->testNetwork->generateAdditionalSegments("FFFF+FFH", 3);
	this->testNetwork->saveTestNetwork("test2.txt");
	this->testNetwork->generateAdditionalSegments("HFF-FF-F", 10);
	this->testNetwork->cleanRoadNetwork();
	this->testNetwork->saveTestNetwork("test3.txt");

	this->testNetwork->loadTestNetwork("test2.txt");
	this->testNetwork->saveTestNetwork("test4.txt");
	
}

void Game::run()
{
	while (this->window->update())
	{
		//Game logic
		//Graphics


		this->graphics.render();
		this->testObject->addRotation(Vector3(0.001, 0.001, 0.001));
	}
	delete this->testObject;
	delete this->testObject2;
	delete this->testNetwork;
}
