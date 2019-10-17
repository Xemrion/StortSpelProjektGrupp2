#include "SkyscraperFloor.h"

SkyscraperFloor::SkyscraperFloor(int edges)
{
	this->center = Vector3(0.0f, 10.0f, 0.0f);
	this->verticies.clear();
	this->nrOfEdges = 0;
	this->generateShape(edges);
}

SkyscraperFloor::SkyscraperFloor(const SkyscraperFloor& other)
{
	this->center = other.center;
	this->verticies.clear();
	this->verticies = other.verticies;
	this->nrOfEdges = other.nrOfEdges;
}

SkyscraperFloor::~SkyscraperFloor()
{
}

void SkyscraperFloor::rotateDeg(float degrees)
{
	float rotX = 0.0f, rotZ = 0.0f;
	float angle = degrees * (pi / 180.0f);
	for (int i = 0; i < this->verticies.size(); i++) {
		rotX = (this->verticies[i].x * float(cos(angle))) + (this->verticies[i].z * float(sin(angle)));
		rotZ = (this->verticies[i].z * float(cos(angle))) - (this->verticies[i].x * float(sin(angle)));
		this->verticies[i].x = rotX;
		this->verticies[i].z = rotZ;
	}
}
void SkyscraperFloor::rotateRad(float radians)
{
	float rotX = 0.0f, rotZ = 0.0f;
	float angle = radians;
	for (int i = 0; i < this->verticies.size(); i++) {
		rotX = (this->verticies[i].x * float(cos(angle))) + (this->verticies[i].z * float(sin(angle)));
		rotZ = (this->verticies[i].z * float(cos(angle))) - (this->verticies[i].x * float(sin(angle)));
		this->verticies[i].x = rotX;
		this->verticies[i].z = rotZ;
	}
}

/*
nextX = (currentForward.x * float((cos(rotationAngle)))) + (currentForward.z * float(sin(rotationAngle)));
nextZ = (currentForward.z * float(cos(rotationAngle))) - (currentForward.x * float(sin(rotationAngle)));
*/

void SkyscraperFloor::generateShape(int edges)
{
	if (edges >= 3) {
		this->nrOfEdges = edges;
		Vector3 temp = this->center + Vector3(0.0f, 0.0f, 5.0f);
		float angle = ((pi * 2) / float(edges));
		this->verticies.push_back(temp);
		for (int i = 1; i < edges; i++) {
			float rotX = (temp.x * float(cos(angle))) + (temp.z * float(sin(angle)));
			float rotZ = (temp.z * float(cos(angle))) - (temp.x * float(sin(angle)));
			temp = Vector3(rotX, temp.y, rotZ);
			this->verticies.push_back(temp);
		}
	}
}

Vector3 SkyscraperFloor::intersectingLines(Vector3& pointA1, Vector3& pointA2, Vector3& pointB1, Vector3& pointB2)
{ //Incorrect: Fix it
	float divider, t1, t2;
	Vector3 result(0.0f, -1.0f, 0.0f);

	divider = (((pointB2.x - pointB1.x) * (pointA1.z - pointA2.z))
		- ((pointA1.x - pointA2.x) * (pointB2.z - pointB1.z)));

	if (divider != 0.0f) {

		t1 = (((pointB1.z - pointB2.z) * (pointA1.x - pointB1.x)) +
			((pointB2.x - pointB1.x) * (pointA1.z - pointB1.z)))
			/ divider;

		t2 = (((pointA1.z - pointA2.z) * (pointA1.x - pointB1.x)) +
			((pointA2.x - pointA1.x) * (pointA1.z - pointB1.z)))
			/ divider;

		if (t1 > 0 && t2 > 0 && t1 < 1 && t2 < 1) {
			Vector3 aResult = Vector3(pointA1.x + (pointA2.x - pointA1.x) * t1, pointA1.y, pointA1.z + (pointA2.z - pointA1.z) * t1);
			Vector3 bResult = Vector3(pointB1.x + (pointB2.x - pointB1.x) * t2, pointB1.y, pointB1.z + (pointB2.z - pointB1.z) * t2);
			result = aResult;
		}
	}
	return result;
}

bool SkyscraperFloor::evenOddCheck( Vector3 pointB1,Vector3 pointB2, Vector3 pointA)
{
	bool intersect = false;
	
	float divider, t1, t2;
	Vector3 direction(0.0f, 0.0f, 1.0f);

	divider = (((pointB2.x - pointB1.x) * (pointA.z - direction.z))
		- ((pointA.x - direction.x) * (pointB2.z - pointB1.z)));

	if (divider != 0) {

		t1 = (((pointB1.z - pointB2.z) * (pointA.x - pointB1.x)) +
			((pointB2.x - pointB1.x) * (pointA.z - pointB1.z)))
			/ divider;

		t2 = (((pointA.z - direction.z) * (pointA.x - pointB1.x)) +
			((direction.x - pointA.x) * (pointA.z - pointB1.z)))
			/ divider;

		if (t1 > 0 && t2 > 0 && t2 < 1) {
			intersect = true;
		}
	}
	
	return intersect;
}


void SkyscraperFloor::unionShapes(SkyscraperFloor& toUnion, Vector3 newCenter)
{
	//Phase 1: Intersections.
	toUnion.translate(newCenter);
	std::vector<Vector3> intersections;
	Vector3 intersectionA(0.0f, 0.0f, 0.0f);
	for (int i = 0; i < toUnion.nrOfEdges; i++) {
		for (int j = 0; j < this->nrOfEdges; j++) {
			if (i == toUnion.nrOfEdges - 1) {
				if (j == this->nrOfEdges - 1) {
					intersectionA = intersectingLines(toUnion.verticies[i], toUnion.verticies[0], this->verticies[j], this->verticies[0]);
				}
				else {
					intersectionA = intersectingLines(toUnion.verticies[i], toUnion.verticies[0], this->verticies[j], this->verticies[size_t(j) + 1]);
				}
			}
			else {
				if (j == this->nrOfEdges - 1) {
					intersectionA = intersectingLines(toUnion.verticies[i], toUnion.verticies[size_t(i) + 1], this->verticies[j], this->verticies[0]);
				}
				else {
					intersectionA = intersectingLines(toUnion.verticies[i], toUnion.verticies[size_t(i) + 1], this->verticies[j], this->verticies[size_t(j) + 1]);
				}
			}
			if (intersectionA.y != -1.0f) {
				intersections.push_back(intersectionA);
				this->verticies.insert(this->verticies.begin() + j + 1, intersectionA);
				this->nrOfEdges++;
				toUnion.verticies.insert(toUnion.verticies.begin() + i + 1, intersectionA);
				toUnion.nrOfEdges++;
			}
		}
	}
	if (intersections.size() > 0) {
		//Even-Odd Rule
		int evenOddCount;
		bool found;
		//Remove points with an odd number of intersections with the other shape. (Odd = inside, Even = outside)
		for (int i = 0; i < this->nrOfEdges; i++) { //Check points in this
			found = false;
			evenOddCount = 0;
			for (int k = 0; k < intersections.size(); k++) {
				if (this->verticies[i] == intersections[k]) {
					found = true;
				}
			}
			if (!found) {
				for (int j = 0; j < toUnion.nrOfEdges; j++) {
					if (j == toUnion.nrOfEdges - 1) {
						if (evenOddCheck(toUnion.verticies[j], toUnion.verticies[0], this->verticies[i])) {
							evenOddCount++;
						}
					}
					else {
						if (evenOddCheck(toUnion.verticies[j], toUnion.verticies[size_t(j) + 1], this->verticies[i])) {
							evenOddCount++;
						}
					}
				}
				if (evenOddCount % 2 == 1) {
					this->verticies.erase(this->verticies.begin() + i);
					this->nrOfEdges--;
					i--;
				}
			}
		}
		for (int i = 0; i < toUnion.nrOfEdges; i++) { //Check points in toUnion
			found = false;
			evenOddCount = 0;
			for (int k = 0; k < intersections.size(); k++) {
				if (toUnion.verticies[i] == intersections[k]) {
					found = true;
				}
			}
			if (!found) {
				for (int j = 0; j < this->nrOfEdges; j++) {
					if (j == this->nrOfEdges - 1) {
						if (evenOddCheck(this->verticies[j], this->verticies[0], toUnion.verticies[i])) {
							evenOddCount++;
						}
					}
					else {
						if (evenOddCheck(this->verticies[j], this->verticies[size_t(j) + 1], toUnion.verticies[i])) {
							evenOddCount++;
						}
					}
				}
				if (evenOddCount % 2 == 1) {
					toUnion.verticies.erase(toUnion.verticies.begin() + i);
					toUnion.nrOfEdges--;
					i--;
				}
			}
		}

		//Add them together
		std::vector<std::pair<int,int>> indicesOfIntersections;
		indicesOfIntersections.resize(intersections.size());
		for (int i = 0; i < intersections.size(); i++) { 
			for (int j = 0; j < toUnion.nrOfEdges; j++) {
				if (intersections[i] == toUnion.verticies[j]) {
					indicesOfIntersections[i].first = j; //Save index of intersection to other shape
				}
			}
			for (int j = 0; j < this->nrOfEdges; j++) {
				if (intersections[i] == this->verticies[j]) {
					indicesOfIntersections[i].second = j; //Save index of intersection to this shape
				}
			}
		}
		if (indicesOfIntersections[0].first == 0) { //If first is 0, then it is connected to second
			for (int i = 0; i < indicesOfIntersections.size(); i+=2) {
				this->verticies.insert(
						this->verticies.begin() + indicesOfIntersections[i].second + 1,
						toUnion.verticies.begin() + indicesOfIntersections[i].first + 1,
						toUnion.verticies.begin() + size_t(indicesOfIntersections[size_t(i) + 1].first));
			}
		}
		else { //If first is not 0, then first and last and connected.
			for (int i = 1; i < indicesOfIntersections.size(); i+=2) {
				if (i == indicesOfIntersections.size() - 1) {
					this->verticies.insert(
						this->verticies.begin() + indicesOfIntersections[i].second + 1,
						toUnion.verticies.begin() + indicesOfIntersections[i].first + 1,
						toUnion.verticies.end());
					this->verticies.insert(
						this->verticies.begin() + indicesOfIntersections[i].second + 1 + ((size_t(toUnion.nrOfEdges) - 1) - (indicesOfIntersections[i].first)),
						toUnion.verticies.begin(),
						toUnion.verticies.begin() + indicesOfIntersections[0].first);
				}
				else {
					this->verticies.insert(
						this->verticies.begin() + indicesOfIntersections[i].second + 1,
						toUnion.verticies.begin() + indicesOfIntersections[i].first,
						toUnion.verticies.begin() + (size_t(indicesOfIntersections[size_t(i) + 1].first) - 1));
				}
			}
		}
		this->nrOfEdges += toUnion.nrOfEdges - int(intersections.size());

	}
	else {
		//Even-Odd Rule on one point in 
		int evenOddCount = 0;
		for (int i = 0; i < toUnion.nrOfEdges; i++) {
			evenOddCount = 0;
			if (i == toUnion.nrOfEdges - 1) {
				if (evenOddCheck(toUnion.verticies[i], toUnion.verticies[0], this->verticies[0])) {
					evenOddCount++;
				}
			}
			else {
				if (evenOddCheck(toUnion.verticies[i], toUnion.verticies[size_t(i) + 1], this->verticies[0])) {
					evenOddCount++;
				}
			}
		}if (evenOddCount % 2 == 1) {
			this->verticies.clear();
			this->verticies = toUnion.verticies;
		}
	}
}
void SkyscraperFloor::testDrawLines(Graphics* graphics)
{
	for (int i = 0; i < this->nrOfEdges; i++) {
		if (i == nrOfEdges - 1) {
			graphics->getdebugger()->DrawLine(XMFLOAT3(this->verticies[i].x, this->center.y, this->verticies[i].z),
				XMFLOAT3(this->verticies[0].x, this->center.y, this->verticies[0].z),
				XMFLOAT3(0.0f, 1.0f, 1.0f));
		}
		else {
			graphics->getdebugger()->DrawLine(XMFLOAT3(this->verticies[i].x, this->center.y, this->verticies[i].z),
				XMFLOAT3(this->verticies[size_t(i) + 1].x, this->center.y, this->verticies[size_t(i) + 1].z),
				XMFLOAT3(0.0f, 1.0f, 1.0f));
		}
	}
}
void SkyscraperFloor::testDrawTriangle()
{

}
Vector3 SkyscraperFloor::getAVertex(int vertex)
{
	Vector3 returnVertex;
	returnVertex = this->verticies[(size_t(vertex) - 1) % this->nrOfEdges];
	return returnVertex;
}
/* Union
	This Shape A
	Other Shape B
	Go through lines in A and see if they intersect with lines from B
	If they intersect, add a point between the lines in A & B.
	After all lines are done, compare points in A to Shape B
	If inside, remove from Copy A
	Repeat for B
	Add points of B between the intersecting points added to A
	You have a union.
*/

void SkyscraperFloor::translate(Vector3 newCenter)
{
	Vector3 translation = newCenter - this->center;
	this->center = this->center + translation;
	for (int i = 0; i < this->nrOfEdges; i++) {
		this->verticies[i] = this->verticies[i] + translation;
	}
	
}

void SkyscraperFloor::regenerateShape(int edges)
{
	this->verticies.clear();
	this->center = Vector3(0.0f, 10.0f, 0.0f);
	this->nrOfEdges = 0;
	generateShape(edges);
}
