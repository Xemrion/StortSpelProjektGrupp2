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
	Vector3 modifyBy(0.0f, 0.5f, 0.0f);
	I32_Dist floorPoints(4, 8), rotation(1, 360), height(4, 9), vectorPoint(0, 500);
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

	modifyBy.y = 2;
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

	modifyBy.y = 1;
	for (int i = 0; i < this->floors.size(); i++) {
		this->floors[i].translateBy(modifyBy);
	}
}

bool Skyscraper::generateSkyscraperMesh()
{
	bool success = false;
	if (this->floors.size() != 0) {
		std::vector<Vertex3D> temp;
		this->mesh.clear();
		for (size_t i = 0; i < this->floors.size() - 1; i++) {
			this->floors[i].getTriangleIndices();
			temp = this->floors[i].getRoofVertices();
			this->mesh.insert(this->mesh.cend(), temp.begin(), temp.end());
			temp = this->floors[i].getWallVertices(this->floors[i + 1].getCenter());
			this->mesh.insert(this->mesh.cend(), temp.begin(), temp.end());
		}
		Vector3 center = this->floors.back().getCenter();
		center.y = 0;
		temp = this->floors.back().getWallVertices(center);
		this->mesh.insert(this->mesh.cend(), temp.begin(), temp.end());
		this->floors.back().getTriangleIndices();
		temp = this->floors.back().getRoofVertices();
		this->mesh.insert(this->mesh.cend(), temp.begin(), temp.end());
		success = true;
	}
	return success;
}

bool Skyscraper::saveSkyscraper(std::string name)
{
	bool success = false;
	if (generateSkyscraperMesh()) {
		Game::getGraphics().loadMesh(name, this->mesh);
		success = true;
	}
	return success;
}

void Skyscraper::testDraw(std::string name)
{
	if (saveSkyscraper(name)) {
		this->building = new GameObject;
		this->building->mesh = Game::getGraphics().getPGMeshPointer(name.c_str());
		Game::getGraphics().addToDraw(this->building);
		this->building->setPosition(Vector3(30.0f, 0.0f, -30.0f));
		this->building->setScale(Vector3(1.0f, 1.0f, 1.0f));
		this->building->setColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		this->building->setTexture(Game::getGraphics().getTexturePointer("brickwall"));
		this->building->setNormalMap(Game::getGraphics().getTexturePointer("brickwallnormal"));
	}
}
