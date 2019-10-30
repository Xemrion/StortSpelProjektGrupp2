#pragma once
#include"SkyscraperFloor.h"
#include"defs.hpp"

class Skyscraper
{
private:
	std::vector<SkyscraperFloor> floors;
	std::vector<Vertex3D> mesh;
	RNG rng { RD() () };
	GameObject* building;

	bool generateSkyscraperMesh();
public:
	Skyscraper(I32 input = -1);
	~Skyscraper();

	void generateSkyscraper();
	bool saveSkyscraper(std::string name);
	void testDraw(std::string name);
};

/* Pseudocode
src <-- random polygon
for every building iteration do
	tmp <-- random polygon
	rotate tmp randomly about y-axis
	translate tmp to random vertex in src
	src <-- src union tmp
*/