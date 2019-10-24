#pragma once
#include"SkyscraperFloor.h"

class Skyscraper
{
private:
	std::vector<SkyscraperFloor> floors;
public:
	Skyscraper(int input);
	~Skyscraper();

	bool generateSkyscraper(int input);
};

/* Pseudocode
src <-- random polygon
for every building iteration do
	tmp <-- random polygon
	rotate tmp randomly about y-axis
	translate tmp to random vertex in src
	src <-- src union tmp
*/