#pragma once
#include<DirectXMath.h>
#include<SimpleMath.h>
#include<vector>
using namespace DirectX::SimpleMath;

class SkyscraperFloor
{
public:
	SkyscraperFloor(int edges = 4);
	SkyscraperFloor(const SkyscraperFloor& other);
	~SkyscraperFloor();

	void rotateDeg(float degrees);
	void rotateRad(float radians);
	void unionShapes(SkyscraperFloor& toUnion);
	void translate(Vector3 newCenter);
private:
	std::vector<Vector3> verticies;
	int nrOfEdges;
	Vector3 center;

	float pi = 3.141592653f;

	void generateShape(int edges); //3 - ?
};


//Sides: n quads

/*
	pick a point
	make a line between neighboors
		if a neighboor is unavailable, move one step further away from start point in the neighboors direction
	make a line from one neighboor to all other points
	cross product to check if any are within triangle
	if yes, change start point and repeat
		if point - unavailable points is two or lower, end loop
	if no, make triangle, set start point to unavailable
*/