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
	if (input > 0) {
		SkyscraperFloor toAdd(5);
		SkyscraperFloor roof(4);

		toAdd.rotateDeg(155);
		roof.unionShapes(toAdd, roof.getAVertex(2));
		this->floors.push_back(roof);

		for (int i = 0; i < this->floors.size(); i++) {
			this->floors[i].translateBy(Vector3(0.0f, 2.0f, 0.0f));
		}

		toAdd.regenerateShape(6);
		toAdd.rotateDeg(35);
		roof.unionShapes(toAdd, roof.getAVertex(5));
		this->floors.push_back(roof);

		for (int i = 0; i < this->floors.size(); i++) {
			this->floors[i].translateBy(Vector3(0.0f, 3.0f, 0.0f));
		}

		toAdd.regenerateShape(4);
		toAdd.rotateDeg(75);
		roof.unionShapes(toAdd, roof.getAVertex(5));
		this->floors.push_back(roof);

		for (int i = 0; i < this->floors.size(); i++) {
			this->floors[i].translateBy(Vector3(0.0f, 5.0f, 0.0f));
		}

		this->floors.push_back(roof);

		for (int i = 0; i < this->floors.size(); i++) {
			this->floors[i].translateBy(Vector3(0.0f, 6.0f, 0.0f));
		}

		success = true;
	}
	return success;
}
