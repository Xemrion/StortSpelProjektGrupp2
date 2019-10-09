#pragma once
#include<d3d11.h>
#include<DirectXMath.h>
#include<SimpleMath.h>
#include<vector>
#include"..//graphics.h"
using namespace DirectX::SimpleMath;

class SkyscraperFloor
{
public:
	SkyscraperFloor(int edges = 4);
	SkyscraperFloor(const SkyscraperFloor& other);
	~SkyscraperFloor();

	void rotateDeg(float degrees);
	void rotateRad(float radians);
	void unionShapes(SkyscraperFloor& toUnion, Vector3 newCenter);
	void testDraw(Graphics* graphics);
private:
	std::vector<Vector3> verticies;
	int nrOfEdges;
	Vector3 center;

	float pi = 3.141592653f;

	void translate(Vector3 newCenter);
	void generateShape(int edges); //3 - ?
	Vector3 intersectingLines(Vector3& pointA1, Vector3& pointA2, Vector3& pointB1, Vector3& pointB2);
	bool evenOddCheck(Vector3 pointB1, Vector3 pointB2, Vector3 pointA);
};


//Sides: n quads

/*	Display a complex shape with triangles

	pick a point
	make a line between neighboors
		if a neighboor is unavailable, move one step further away from start point in the neighboors direction
	make a line from one neighboor to all other points
	cross product to check if any are within triangle
	if yes, change start point and repeat
		if (point - unavailable points) is two or lower, end loop
	if no, make triangle, set start point to unavailable
*/