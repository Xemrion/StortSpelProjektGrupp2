#pragma once
#include"SkyscraperFloor.h"
#include"defs.hpp"

class Skyscraper
{
private:
	std::vector<SkyscraperFloor> floors;
	std::vector<Vertex3D> meshRoof;
	std::vector<Vertex3D> meshWindows;
	std::vector<Vertex3D> meshWalls;
	RNG rng { RD() () };

	bool generateSkyscraperMesh();
	void scrapSkyScraper();
	void generateSkyscraper();
	bool saveSkyscraper(std::string name);
	void unloadRoof(std::string name);
	void unloadWall(std::string name);
	void unloadWindow(std::string name);
public:
	Skyscraper(I32 input = -1);
	~Skyscraper();

	void generateASkyscraper(std::string name);
	void setRoofMesh(std::string name, GameObject* roof);
	void setWallMesh(std::string name, GameObject* roof);
	void setWindowMesh(std::string name, GameObject* roof);
	void unloadSkyscrapers(std::vector<std::string>& names);
	void unloadASkyscraper(std::string names);
};

/* Pseudocode
src <-- random polygon
for every building iteration do
	tmp <-- random polygon
	rotate tmp randomly about y-axis
	translate tmp to random vertex in src
	src <-- src union tmp
*/