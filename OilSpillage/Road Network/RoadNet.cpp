#include "RoadNet.h"


RoadNetwork::RoadNetwork(int randNr, glm::fvec2 max, glm::fvec2 min, float rotation)
{
	this->roadNetworkMax = max;
	this->roadNetworkMin = min;
	this->rotationAngle = rotation;

	//Calculate startpoint
	glm::fvec2 fromMinToMaxDir = (roadNetworkMax - roadNetworkMin);
	glm::fvec2 partway = (glm::fvec2)(fromMinToMaxDir.x / 1000, fromMinToMaxDir.y / 1000);
	
	int rand = randNr % 1000 + 1;
	int xOrY = rand % 2;
	bool toMin;
	if (rand > 500) {
		toMin = false;
	}
	else {
		toMin = true;
	}
	glm::fvec2 possibleStart = (glm::fvec2)( partway.x * rand, partway.y * rand);
	if (xOrY == 0) { //move start to x value
		if (toMin) {
			possibleStart.x = this->roadNetworkMin.x;
			this->forwardDistance = fromMinToMaxDir.y / 20;
			this->forward = (glm::fvec4)(0.0f, forwardDistance, 0.0f, 0.0f);
		}
		else {
			possibleStart.x = this->roadNetworkMax.x;
			this->forwardDistance = fromMinToMaxDir.y / 20;
			this->forward = (glm::fvec4)(0.0f, -forwardDistance, 0.0f, 0.0f);
		}
	}
	else if (xOrY == 1) { //move start to y value
		if (toMin) {
			possibleStart.y = this->roadNetworkMin.y;
			this->forwardDistance = fromMinToMaxDir.x / 20;
			this->forward = (glm::fvec4)(forwardDistance, 0.0f, 0.0f, 0.0f);
		}
		else {
			possibleStart.y = this->roadNetworkMax.y;
			this->forwardDistance = fromMinToMaxDir.x / 20;
			this->forward = (glm::fvec4)(-forwardDistance, 0.0f, 0.0f, 0.0f);
		}
	}
	this->startPoint = (glm::fvec4)(possibleStart, 1.0f, 0.0f);
	Segment startSegment;
	startSegment.firstPoint = this->startPoint - forward;
	startSegment.secondPoint = this->startPoint;
	this->roadNetwork.push_back(startSegment);
}

RoadNetwork::~RoadNetwork()
{

}

void RoadNetwork::setLetters(char forward, char left, char right)
{
	this->forwardSymbol = forward;
	this->turnLeftSymbol = left;
	this->turnRightSymbol = right;
}

void RoadNetwork::setAngle(float angle)
{
	this->rotationAngle = angle;
}

float RoadNetwork::getAngle() const
{
	return this->rotationAngle;
}

void RoadNetwork::clearSegments()
{
	Segment first = this->roadNetwork.front();
	this->roadNetwork.clear();
	this->roadNetwork.push_back(first);
}

bool RoadNetwork::generateInitialSegments(const char* seed)
{
	bool success = false;
	int counter = 0;

	while (seed[counter]) {

		if (seed[counter] == forwardSymbol) {
			Segment temp;
			temp.firstPoint = this->roadNetwork[-1].secondPoint;
			temp.secondPoint = temp.firstPoint + this->forward;
			if (temp.secondPoint.x > this->roadNetworkMax.x) {
				temp.secondPoint.x = this->roadNetworkMax.x;
			}
			else if (temp.secondPoint.x < this->roadNetworkMin.x) {
				temp.secondPoint.x = this->roadNetworkMin.x;
			}
			else if (temp.secondPoint.y > this->roadNetworkMax.y) {
				temp.secondPoint.y = this->roadNetworkMax.y;
			}
			else if (temp.secondPoint.y < this->roadNetworkMin.y) {
				temp.secondPoint.y = this->roadNetworkMin.y;
			}
			this->roadNetwork.push_back(temp);

			success = true;
		}

		if (seed[counter] == turnLeftSymbol) { //Mainline turtle walk
			if (this->forward.x > 0) {
				this->forward.x = 0;
				this->forward.y = forwardDistance;
			}
			else if (this->forward.y > 0) {
				this->forward.y = 0;
				this->forward.x = -forwardDistance;
			}
			else if (this->forward.x < 0) {
				this->forward.x = 0;
				this->forward.y = -forwardDistance;
			}
			else if (this->forward.y < 0) {
				this->forward.y = 0;
				this->forward.x = forwardDistance;
			}

			success = true;
		}

		if (seed[counter] == turnRightSymbol) {
			if (this->forward.x > 0) {
				this->forward.x = 0;
				this->forward.y = -forwardDistance;
			}
			else if (this->forward.y > 0) {
				this->forward.y = 0;
				this->forward.x = forwardDistance;
			}
			else if (this->forward.x < 0) {
				this->forward.x = 0;
				this->forward.y = forwardDistance;
			}
			else if (this->forward.y < 0) {
				this->forward.y = 0;
				this->forward.x = -forwardDistance;
			}

			success = true;
		}

		counter++;
	}
	return success;
}

bool RoadNetwork::generateAdditionalSegments(const char* seed) //Turtle walk from existing segments
{
	bool success = false;
	int counter = 0;

	while (seed[counter]) {

		if (seed[counter] == forwardSymbol) {
			Segment temp;
			temp.firstPoint = this->roadNetwork[-1].secondPoint;
			temp.secondPoint = temp.firstPoint + this->forward;
			if (temp.secondPoint.x > this->roadNetworkMax.x) {
				temp.secondPoint.x = this->roadNetworkMax.x;
			}
			else if (temp.secondPoint.x < this->roadNetworkMin.x) {
				temp.secondPoint.x = this->roadNetworkMin.x;
			}
			else if (temp.secondPoint.y > this->roadNetworkMax.y) {
				temp.secondPoint.y = this->roadNetworkMax.y;
			}
			else if (temp.secondPoint.y < this->roadNetworkMin.y) {
				temp.secondPoint.y = this->roadNetworkMin.y;
			}
			this->roadNetwork.push_back(temp);

			success = true;
		}

		if (seed[counter] == turnLeftSymbol) {
			if (this->forward.x > 0) {
				this->forward.x = 0;
				this->forward.y = forwardDistance;
			}
			else if (this->forward.y > 0) {
				this->forward.y = 0;
				this->forward.x = -forwardDistance;
			}
			else if (this->forward.x < 0) {
				this->forward.x = 0;
				this->forward.y = -forwardDistance;
			}
			else if (this->forward.y < 0) {
				this->forward.y = 0;
				this->forward.x = forwardDistance;
			}

			success = true;
		}

		if (seed[counter] == turnRightSymbol) {
			if (this->forward.x > 0) {
				this->forward.x = 0;
				this->forward.y = -forwardDistance;
			}
			else if (this->forward.y > 0) {
				this->forward.y = 0;
				this->forward.x = forwardDistance;
			}
			else if (this->forward.x < 0) {
				this->forward.x = 0;
				this->forward.y = forwardDistance;
			}
			else if (this->forward.y < 0) {
				this->forward.y = 0;
				this->forward.x = -forwardDistance;
			}

			success = true;
		}

		counter++;
	}
	return success;
}
