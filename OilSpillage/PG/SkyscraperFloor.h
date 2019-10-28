#pragma once
#include<d3d11.h>
#include<SimpleMath.h>
#include<vector>
#include"..//Structs.h"
#include"..//game.h"
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
	Vector3 getAVertex(int vertex) const;
	Vector3 getCenter() const;
	void translate(Vector3 newCenter);
	void translateBy(Vector3 moveBy);
	void scale(Vector3 scaleBy);
	void regenerateShape(int edges);

	void testDrawLines() const; //Lines
	void getTriangleIndices();
	std::vector<Vertex3D> getWallVertices(Vector3 otherCenter);
	std::vector<Vertex3D> getRoofVertices();
	void testDrawTriangles(std::string name, Vector4 colour);
private:
	std::vector<Vector3> verticies;
	std::vector<int> indices;
	int nrOfEdges;
	Vector3 center;
	GameObject* roof;

	float pi = 3.141592653f;


	void generateShape(int edges);
	Vector3 intersectingLines(Vector3& pointA1, Vector3& pointA2, Vector3& pointB1, Vector3& pointB2) const;
	bool evenOddCheck(Vector3 pointB1, Vector3 pointB2, Vector3 pointA) const;
};

/*	Display a complex shape with triangles

	pick a point
	make a line between neighboors
		if a neighboor is unavailable, move one step further away from start point in the neighboors direction
		repeat until both neighboor are available
	make a line from one neighboor to all other points
	cross product to check the side the start point is one
	cross product with the other ones, if any other is on the same side as the start point, it cannot be
	if yes, change start point and repeat
		if (point - unavailable points) is two or lower, end loop
	if no, make triangle, set start point to unavailable
*/

/*
	Get a lower roof.

	Compare two vectors of points
	if a point doesn't exist in both, add to a new vector of points
	save index of the points, compare one below the lowest index, if it exists in both, add it
	repeat for one after the highest index
	this vector is the lower roof
*/

/*
	Making sides

	compare bottom to top
	if the point doesn't exist, compare to a lower floor
	it the point exists, save index of both and the roof it is on
*/