#include "Skyscraper.h"

Skyscraper::Skyscraper(I32 input)
{
	if (input != -1) {
		rng.seed(input);
	}
	generateSkyscraper();
}

Skyscraper::~Skyscraper()
{
}

void Skyscraper::generateSkyscraper()
{
	this->floors.clear();
	Vector3 modifyBy(0.0f, 2.0f, 0.0f);
	I32_Dist floorPoints(4, 8), rotation(1, 360), height(4, 9), vectorPoint(1, 500);
	I32_Dist floors(2 , 4);
	int nrOfFloors = 0;
	//Random
	SkyscraperFloor toAdd(floorPoints(rng));
	SkyscraperFloor roof(floorPoints(rng));

	//Random
	toAdd.rotateDeg(rotation(rng));
	roof.unionShapes(toAdd, roof.getAVertex((vectorPoint(rng))));

	SkyscraperFloor temp(roof);
	temp.scale(Vector3(0.8f, 1.0f, 0.8f));
	this->floors.push_back(temp);
	this->floors[0].translateBy(modifyBy);
	this->floors.push_back(roof);

	modifyBy.y = height(rng);
	for (int i = 0; i < this->floors.size(); i++) {
		this->floors[i].translateBy(modifyBy);
	}

	
	//Random
	toAdd.regenerateShape(floorPoints(rng));
	toAdd.rotateDeg(rotation(rng));
	roof.unionShapes(toAdd, roof.getAVertex(vectorPoint(rng)));
	this->floors.push_back(roof);

	//Random
	modifyBy.y = height(rng);
	for (int i = 0; i < this->floors.size(); i++) {
		this->floors[i].translateBy(modifyBy);
	}

	//Random
	toAdd.regenerateShape(floorPoints(rng));
	toAdd.rotateDeg(rotation(rng));
	roof.unionShapes(toAdd, roof.getAVertex(vectorPoint(rng)));
	this->floors.push_back(roof);

	//Random
	modifyBy.y = height(rng);
	for (int i = 0; i < this->floors.size(); i++) {
		this->floors[i].translateBy(modifyBy);
	}
		
	this->floors.push_back(roof);

	modifyBy.y = 2.0f;
	for (int i = 0; i < this->floors.size(); i++) {
		this->floors[i].translateBy(modifyBy);
	}
}

bool Skyscraper::generateSkyscraperMesh()
{
	bool success = false;
	if (this->floors.size() != 0) {
		std::vector<Vertex3D> temp;
		this->meshRoof.clear();
		this->meshWindows.clear();
		this->meshWalls.clear();

		this->floors[0].getTriangleIndices();
		temp = this->floors[0].getRoofVertices();
		this->meshRoof.insert(this->meshRoof.cend(), temp.begin(), temp.end());
		temp = this->floors[0].getWallVertices(this->floors[1].getCenter());
		this->meshWalls.insert(this->meshWalls.cend(), temp.begin(), temp.end());

		this->floors[1].getTriangleIndices();
		temp = this->floors[1].getRoofVertices();
		this->meshRoof.insert(this->meshRoof.cend(), temp.begin(), temp.end());
		temp = this->floors[1].getWindowVertices(this->floors[2].getCenter());
		this->meshWindows.insert(this->meshWindows.cend(), temp.begin(), temp.end());

		for (size_t i = 2; i < this->floors.size() - 1; i++) {
			
			temp = this->floors[i].getDifferenceAsRoofVerticies(this->floors[i - 1]);
			this->meshRoof.insert(this->meshRoof.cend(), temp.begin(), temp.end());
			temp = this->floors[i].getWindowVertices(this->floors[i + 1].getCenter());
			this->meshWindows.insert(this->meshWindows.cend(), temp.begin(), temp.end());
		}
		Vector3 center = this->floors.back().getCenter();
		center.y = 0;
		temp = this->floors.back().getWallVertices(center);
		this->meshWalls.insert(this->meshWalls.cend(), temp.begin(), temp.end());
		success = true;
	}
	return success;
}

bool Skyscraper::saveSkyscraper(std::string name)
{
	bool success = false;
	if (generateSkyscraperMesh()) {
		Game::getGraphics().loadMesh(name + "-roof", this->meshRoof);
		Game::getGraphics().loadMesh(name + "-wind", this->meshWindows);
		Game::getGraphics().loadMesh(name + "-wall", this->meshWalls);
		success = true;
	}
	return success;
}

void Skyscraper::testDraw(std::string name)
{
	if (saveSkyscraper(name)) {
		this->roof = new GameObject;
		std::string ss_roof = name + "-roof", ss_windows = name + "-wind", ss_walls = name + "-wall";
		this->roof->mesh = Game::getGraphics().getPGMeshPointer(ss_roof.c_str());
		this->windows = new GameObject;
		this->windows->mesh = Game::getGraphics().getPGMeshPointer(ss_windows.c_str());
		this->walls = new GameObject;
		this->walls->mesh = Game::getGraphics().getPGMeshPointer(ss_walls.c_str());
		Game::getGraphics().addToDraw(this->roof);
		Game::getGraphics().addToDraw(this->windows);
		Game::getGraphics().addToDraw(this->walls);
		this->roof->setPosition(Vector3(30.0f, 0.0f, -30.0f));
		this->roof->setScale(Vector3(1.0f, 1.0f, 1.0f));
		this->roof->setColor(Vector4(0.2f, 0.2f, 0.1f, 1.0f));
		this->roof->setTexture(Game::getGraphics().getTexturePointer("brickwall"));
		this->roof->setNormalMap(Game::getGraphics().getTexturePointer("brickwallnormal"));

		this->windows->setPosition(Vector3(30.0f, 0.0f, -30.0f));
		this->windows->setScale(Vector3(1.0f, 1.0f, 1.0f));
		this->windows->setColor(Vector4(0.2f, 0.2f, 0.1f, 1.0f));
		this->windows->setTexture(Game::getGraphics().getTexturePointer("fireball"));
		this->windows->setNormalMap(Game::getGraphics().getTexturePointer("brickwallnormal"));

		this->walls->setPosition(Vector3(30.0f, 0.0f, -30.0f));
		this->walls->setScale(Vector3(1.0f, 1.0f, 1.0f));
		this->walls->setColor(Vector4(0.2f, 0.2f, 0.1f, 1.0f));
		this->walls->setTexture(Game::getGraphics().getTexturePointer("testWood"));
		this->walls->setNormalMap(Game::getGraphics().getTexturePointer("brickwallnormal"));
	}
}
