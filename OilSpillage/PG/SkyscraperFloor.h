#pragma once

class SkyscraperFloor
{
public:
	SkyscraperFloor(int edges = 4, float rotationAngle = 0);
	~SkyscraperFloor();

	void generatePrimitive(int edges, float rotationAngle);
	void rotatePrimitive(float rotationAngle);
	void unionPrimitive(SkyscraperFloor& toUnion);
	void translatePrimitive()
private:

};