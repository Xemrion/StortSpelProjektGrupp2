#pragma once

class Skyscraper
{
private:

public:
	Skyscraper();
	~Skyscraper();

};

/* Pseudocode
src <-- random polygon
for every building iteration do
	tmp <-- random polygon
	rotate tmp randomly about y-axis
	translate tmp to random vertex in src
	src <-- src union tmp
*/