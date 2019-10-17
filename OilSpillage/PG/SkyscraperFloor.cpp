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
void SkyscraperFloor::testDrawLines() const
{
	
	for (int i = 0; i < this->indices.size(); i += 3) {
		Game::getGraphics().getdebugger()->DrawLine(XMFLOAT3(this->verticies[this->indices[i]].x, this->center.y, this->verticies[this->indices[i]].z),
			XMFLOAT3(this->verticies[this->indices[(size_t(i) + 1)]].x, this->center.y, this->verticies[this->indices[(size_t(i) + 1)]].z),
			XMFLOAT3(0.0f, 1.0f, 1.0f));
		Game::getGraphics().getdebugger()->DrawLine(XMFLOAT3(this->verticies[this->indices[(size_t(i) + 1)]].x, this->center.y, this->verticies[this->indices[(size_t(i) + 1)]].z),
			XMFLOAT3(this->verticies[this->indices[(size_t(i) + 2)]].x, this->center.y, this->verticies[this->indices[(size_t(i) + 2)]].z),
			XMFLOAT3(0.0f, 1.0f, 1.0f));
		Game::getGraphics().getdebugger()->DrawLine(XMFLOAT3(this->verticies[this->indices[(size_t(i) + 2)]].x, this->center.y, this->verticies[this->indices[(size_t(i) + 2)]].z),
			XMFLOAT3(this->verticies[this->indices[i]].x, this->center.y, this->verticies[this->indices[i]].z),
			XMFLOAT3(0.0f, 1.0f, 1.0f));
	}
}

void SkyscraperFloor::getTriangleIndices()
{
	std::vector<int> triangleIndices;
	std::vector<int> usedIndices;
	int counter = 1, forwardsOffset = 1, backwardsOffset = 1;
	size_t size = this->verticies.size();
	bool counterUsed = false, forwardsUsed = false, backwardsUsed = false;
	bool mainPointLeft = false;
	int pointsOnMainPointSide = 0;

	while (size - usedIndices.size() > 2) {
		for (int i = 0; i < usedIndices.size() && !counterUsed; i++) {
			if (counter == usedIndices[i]) {
				counterUsed = true;
			}
		}
		if (!counterUsed) {
			Vector3 backToFront, backToMainPoint, backToOther, crossProduct;
			bool towardsUsed = false;
			//Check forward until an available is found, save offset from point
			do {
				forwardsUsed = false;
				for (int i = 0; i < usedIndices.size() && !forwardsUsed; i++) {
					if (usedIndices[i] == (size_t(counter) + size_t(forwardsOffset)) % size) {
						forwardsUsed = true;
					}
				}
				if (forwardsUsed) {
					forwardsOffset++;
				}
			} while (forwardsUsed);

			//Check backward until an available is found, save offset from point
			do {
				backwardsUsed = false;
				for (int i = 0; i < usedIndices.size() && !backwardsUsed; i++) {
					if (usedIndices[i] == (size_t(counter) - size_t(backwardsOffset) + size) % size) {
						backwardsUsed = true;
					}
				}
				if (backwardsUsed) {
					backwardsOffset++;
				}
			} while (backwardsUsed);
			
			//Make a line from backward to forward and backward to main point
			backToFront = this->verticies[(size_t(counter) + size_t(forwardsOffset)) % size] - this->verticies[(size_t(counter) - size_t(backwardsOffset) + size) % size];
			backToMainPoint = this->verticies[counter] - this->verticies[(size_t(counter) - size_t(backwardsOffset) + size) % size];
			backToFront.Normalize();
			backToMainPoint.Normalize();
			//Cross product the start point line first
			crossProduct = backToFront.Cross(backToMainPoint);
			if (crossProduct.y > 0) {
				mainPointLeft = false;
				pointsOnMainPointSide++;
			}
			else if(crossProduct.y < 0) {
				mainPointLeft = true;
				pointsOnMainPointSide++;
			}
			//Cross product the start point line first, then others, if any are on the same side as the point line, there cannot be a triangle
			for (int i = 1; i < size - (size_t(backwardsOffset) + size_t(forwardsOffset)) && pointsOnMainPointSide < 2; i++) {
				towardsUsed = false;
				for (int j = 0; j < usedIndices.size() && !towardsUsed; j++) {
					if ((i + size_t(counter) + size_t(forwardsOffset)) % size == usedIndices[j]) {
						towardsUsed = true;
					}
				}
				if (!towardsUsed) {
					backToOther = this->verticies[(i + size_t(counter) + size_t(forwardsOffset)) % size] - this->verticies[(size_t(counter) - size_t(backwardsOffset) + size) % size];
					backToOther.Normalize();
					crossProduct = backToFront.Cross(backToOther);
					if (!mainPointLeft) {
						if (crossProduct.y > 0) {
							pointsOnMainPointSide++;
						}
					}
					else {
						if (crossProduct.y < 0) {
							pointsOnMainPointSide++;
						}
					}
				}
			}
			if (pointsOnMainPointSide < 2) {
				triangleIndices.push_back(int((size_t(counter) - size_t(backwardsOffset) + size) % size));
				triangleIndices.push_back(counter);
				triangleIndices.push_back((size_t(counter) + size_t(forwardsOffset)) % size);
				usedIndices.push_back(counter);
			}

			pointsOnMainPointSide = 0;
		}

		counter++;
		counterUsed = false;
		if (counter > size) {
			counter -= int(size);
		}
	}
	this->indices = triangleIndices;
}

std::vector<Vertex3D> SkyscraperFloor::getVertices()
{
	std::vector<Vertex3D> meshData;
	Vertex3D temp;
	float toNorm = 0.0f;
	if (this->indices.size() == 0) {
		this->getTriangleIndices();
	}
	for (int i = 0; i < this->indices.size(); i += 3) {
		temp.position = this->verticies[this->indices[i]];
		temp.normal = Vector3(0.0f, 1.0f, 0.0f);
		toNorm = 1.0f / (this->verticies[this->indices[i]].x + this->verticies[this->indices[i]].z);
		temp.uv = Vector2(this->verticies[this->indices[i]].x * toNorm, this->verticies[this->indices[i]].z * toNorm);
		meshData.push_back(temp);

		temp.position = this->verticies[this->indices[size_t(i) + 1]];
		temp.normal = Vector3(0.0f, 1.0f, 0.0f);
		toNorm = 1.0f / (this->verticies[this->indices[size_t(i) + 1]].x + this->verticies[this->indices[size_t(i) + 1]].z);
		temp.uv = Vector2(this->verticies[this->indices[size_t(i) + 1]].x * toNorm, this->verticies[this->indices[size_t(i) + 1]].z * toNorm);
		meshData.push_back(temp);

		temp.position = this->verticies[this->indices[size_t(i) + 2]];
		temp.normal = Vector3(0.0f, 1.0f, 0.0f);
		toNorm = 1.0f / (this->verticies[this->indices[size_t(i) + 2]].x + this->verticies[this->indices[size_t(i) + 2]].z);
		temp.uv = Vector2(this->verticies[this->indices[size_t(i) + 2]].x * toNorm, this->verticies[this->indices[size_t(i) + 2]].z * toNorm);
		meshData.push_back(temp);
	}
	return meshData;
}

void SkyscraperFloor::testDrawTriangles()
{
	std::vector<Vertex3D> meshData;
	meshData = this->getVertices();
	this->roof = new GameObject();
	Mesh test;
	test.insertDataToMesh(meshData);
	this->roof->mesh = new Mesh(test);
	Game::getGraphics().addToDraw(this->roof);
	this->roof->setPosition(this->center);
	this->roof->setScale(Vector3(1.0f, 1.0f, 1.0f));
	Game::getGraphics().loadTexture("brickwall");
	this->roof->setTexture(Game::getGraphics().getTexturePointer("brickwall"));
}

Vector3 SkyscraperFloor::getAVertex(int vertex)
{
	Vector3 returnVertex;
	returnVertex = this->verticies[(size_t(vertex) - 1) % this->nrOfEdges];
	return returnVertex;
}

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
