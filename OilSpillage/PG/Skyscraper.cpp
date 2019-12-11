#include "Skyscraper.h"

Skyscraper::Skyscraper(I32 input)
{
	if (input != -1) {
		rng.seed(input);
	}
}

Skyscraper::~Skyscraper()
{
}

void Skyscraper::generateASkyscraper(std::string name)
{
	bool success = false;
	do {
		if (this->floors.size() == 0) {
			generateSkyscraper();
		}
		if (saveSkyscraper(name)) {
			success = true;
		}
		else {
			scrapSkyScraper();
			generateSkyscraper();
		}
	} while (!success);
	scrapSkyScraper();
}

void Skyscraper::setRoofMesh(std::string name, DynamicGameObject* roof)
{
	std::string meshName = name + "-roof";
	roof->mesh = Game::getGraphics().getPGMeshPointer(meshName.c_str());
}

void Skyscraper::setWallMesh(std::string name, DynamicGameObject* roof)
{
	std::string meshName = name + "-wall";
	roof->mesh = Game::getGraphics().getPGMeshPointer(meshName.c_str());
}

void Skyscraper::setWindowMesh(std::string name, DynamicGameObject* roof)
{
	std::string meshName = name + "-wind";
	roof->mesh = Game::getGraphics().getPGMeshPointer(meshName.c_str());
}

void Skyscraper::unloadSkyscrapers(std::vector<std::string>& names)
{
	for (int i = 0; i < names.size(); i++) {
		unloadRoof(names[i]);
		unloadWall(names[i]);
		unloadWindow(names[i]);
	}
	names.clear();
}

void Skyscraper::unloadASkyscraper(std::string names)
{
	unloadRoof(names);
	unloadWall(names);
	unloadWindow(names);
	
	names.clear();
}

void Skyscraper::unloadRoof(std::string name)
{
	std::string meshName = name + "-roof";
	Game::getGraphics().unloadMesh(meshName);
}

void Skyscraper::unloadWall(std::string name)
{
	std::string meshName = name + "-wall";
	Game::getGraphics().unloadMesh(meshName);
}

void Skyscraper::unloadWindow(std::string name)
{
	std::string meshName = name + "-wind";
	Game::getGraphics().unloadMesh(meshName);
}

void Skyscraper::generateSkyscraper()
{
	this->floors.clear();
	Vector3 modifyBy(0.0f, 2.0f, 0.0f);
	I32_Dist floorPoints(4, 8), rotation(1, 360), height(3, 8), vectorPoint(1, 500);
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
		success = true;
		std::vector<Vertex3D> temp;
		this->meshRoof.clear();
		this->meshWindows.clear();
		this->meshWalls.clear();

		if (!this->floors[0].getTriangleIndices()) {
			success = false;
		}
		temp = this->floors[0].getRoofVertices();
		this->meshRoof.insert(this->meshRoof.cend(), temp.begin(), temp.end());
		temp = this->floors[0].getWallVertices(this->floors[1].getCenter());
		this->meshWalls.insert(this->meshWalls.cend(), temp.begin(), temp.end());

		if (!this->floors[1].getTriangleIndices()) {
			success = false;
		}
		temp = this->floors[1].getRoofVertices();
		this->meshRoof.insert(this->meshRoof.cend(), temp.begin(), temp.end());
		temp = this->floors[1].getWindowVertices(this->floors[2].getCenter());
		this->meshWindows.insert(this->meshWindows.cend(), temp.begin(), temp.end());

		for (size_t i = 2; i < this->floors.size() - 1; i++) {
			
			temp = this->floors[i].getDifferenceAsRoofVerticies(this->floors[i - 1]);
			if (temp.size() == 0) {
				success = false;
			}
			else {
				this->meshRoof.insert(this->meshRoof.cend(), temp.begin(), temp.end());
			}
			temp = this->floors[i].getWindowVertices(this->floors[i + 1].getCenter());
			this->meshWindows.insert(this->meshWindows.cend(), temp.begin(), temp.end());
		}
		Vector3 center = this->floors.back().getCenter();
		center.y = 0;
		temp = this->floors.back().getWallVertices(center);
		this->meshWalls.insert(this->meshWalls.cend(), temp.begin(), temp.end());
	}
	return success;
}

void Skyscraper::scrapSkyScraper()
{
	this->floors.clear();
	this->meshRoof.clear();
	this->meshWalls.clear();
	this->meshWindows.clear();
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

