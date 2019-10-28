#include "Skyscraper.h"

Skyscraper::Skyscraper(int input)
{
	generateSkyscraper(input);
}

Skyscraper::~Skyscraper()
{
}

bool Skyscraper::generateSkyscraper(int input)
{
	bool success = false;
	this->floors.clear();
	Vector3 modifyBy(0.0f, 0.5f, 0.0f);
	int randomNr = 0; //Use for random numbers

	if (input > 0) {
		//Random
		SkyscraperFloor toAdd(5);
		SkyscraperFloor roof(4);

		//Random
		toAdd.rotateDeg(155);
		roof.unionShapes(toAdd, roof.getAVertex(2));

		SkyscraperFloor temp(roof);
		temp.scale(Vector3(0.9f, 1.0f, 0.9f));
		this->floors.push_back(temp);
		this->floors[0].translateBy(modifyBy);
		this->floors.push_back(roof);

		//Random
		modifyBy.y = 2;
		for (int i = 0; i < this->floors.size(); i++) {
			this->floors[i].translateBy(modifyBy);
		}

		//Random
		toAdd.regenerateShape(6);
		toAdd.rotateDeg(35);
		roof.unionShapes(toAdd, roof.getAVertex(5));
		this->floors.push_back(roof);

		//Random
		modifyBy.y = 3;
		for (int i = 0; i < this->floors.size(); i++) {
			this->floors[i].translateBy(modifyBy);
		}

		//Random
		toAdd.regenerateShape(4);
		toAdd.rotateDeg(75);
		roof.unionShapes(toAdd, roof.getAVertex(5));
		this->floors.push_back(roof);

		//Random
		modifyBy.y = 4;
		for (int i = 0; i < this->floors.size(); i++) {
			this->floors[i].translateBy(modifyBy);
		}

		this->floors.push_back(roof);

		//Random
		modifyBy.y = 3;
		for (int i = 0; i < this->floors.size(); i++) {
			this->floors[i].translateBy(modifyBy);
		}

		success = true;
	}
	return success;
}

void Skyscraper::generateSkyscraperMesh()
{
	std::vector<Vertex3D> temp;
	this->mesh.clear();
	for (int i = 0; i < this->floors.size() - 1; i++) {
		this->floors[i].getTriangleIndices();
		temp = this->floors[i].getRoofVertices();
		this->mesh.insert(this->mesh.cend() + 1, temp.begin(), temp.end());
		temp = this->floors[i].getWallVertices(this->floors[i + 1].getCenter());
		this->mesh.insert(this->mesh.cend() + 1, temp.begin(), temp.end());
	}
}

void Skyscraper::saveSkyscraper(std::string name)
{
	Game::getGraphics().loadMesh(name.c_str(), this->mesh);
}
