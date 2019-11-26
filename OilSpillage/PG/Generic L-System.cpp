#include "Generic L-System.h"

Lsystem::Lsystem()
{
	this->axiom = "";
	this->result = "";
	this->forWardDistance = 1;
	this->forwardDirection = Vector3(0.0f, 0.0f, 1.0f);
	this->startPosition = Vector3(0.0f);
	this->angle = 90.0f * ((3.1415926535f) / 180.0f);
	this->recursions = 0;
	this->valuesModified = true;
	this->rulesModified = true;
}

Lsystem::~Lsystem()
{
	clearSegments();
	constants.clear();
	variables.clear();
	storedPositions.clear();
}

void Lsystem::addConstant(char character, Rule action, bool forward)
{ //Add a constant, requires a char, a Rule type or forward. (Higher priority)
	Constant newConstant;
	newConstant.character = character;
	newConstant.action = action;
	newConstant.forward = forward;
	bool added = false;
	for (int i = 0; i < this->constants.size(); i++) {
		if (this->constants[i].character == newConstant.character) {
			this->constants[i] = newConstant;
			added = true;
		}
	}
	if (!added) {
		this->constants.push_back(newConstant);
	}
	removeVariable(character);
	rulesModified = true;
	valuesModified = true;
}

void Lsystem::removeConstant(char character)
{ //Removes a constant based on a char
	bool removed = false;
	for (int i = 0; i < this->constants.size() && !removed; i++) {
		if (this->constants[i].character == character) {
			this->constants.erase(this->constants.begin() + i);
			removed = true;
		}
	}
	rulesModified = true;
	valuesModified = true;
}

void Lsystem::addVariable(char character, String replacementString, bool forward)
{ //Add a variable, it needs a char, a string to replace the char, and a bool if it should draw forward or not
	Variable newVariable;
	newVariable.character = character;
	newVariable.replacement = replacementString;
	newVariable.forward = forward;
	bool added = false;
	for (int i = 0; i < this->variables.size(); i++) {
		if (this->variables[i].character == newVariable.character) {
			this->variables[i] = newVariable;
			added = true;
		}
	}
	if (!added) {
		this->variables.push_back(newVariable);
	}
	removeConstant(character);
	rulesModified = true;
	valuesModified = true;
}

void Lsystem::removeVariable(char character)
{ //Removes a variable based on a char
	bool removed = false;
	for (int i = 0; i < this->variables.size() && !removed; i++) {
		if (this->variables[i].character == character) {
			this->variables.erase(this->variables.begin() + i);
			removed = true;
		}
	}
	rulesModified = true;
	valuesModified = true;
}

void Lsystem::setAngleDegrees(float degree)
{//Set angle using degrees
	this->angle = degree * ((3.1415926535f) / 180.0f);
	valuesModified = true;
}

void Lsystem::setAngleRadians(float radians)
{ //Set angle using radians
	this->angle = radians;
	valuesModified = true;
}

void Lsystem::setRecursions(int recursions)
{ //Number of times the replacement function will run
	this->recursions = recursions;
	rulesModified = true;
	valuesModified = true;
}

void Lsystem::setStartPosition(Vector3 startPos)
{ //Set start position
	Vector3 difference = startPos - this->startPosition;
	this->startPosition += difference;
	moveLSystem(difference);
}

void Lsystem::setForwardDirection(Vector3 direction)
{ //Set direction of the start forward
	this->forwardDirection = direction;
	this->forwardDirection.Normalize();
	valuesModified = true;
}

void Lsystem::setForwardDistance(float distance)
{ //Set lenght
	this->forWardDistance = distance;
	valuesModified = true;
}

void Lsystem::setAxiom(String axiom)
{ //Set the starting string
	this->axiom = axiom;
	rulesModified = true;
	valuesModified = true;
}

void Lsystem::updateLSystem()
{
	if (rulesModified) {
		this->result = applyRules(this->axiom, this->recursions);
		rulesModified = false;
	}
	if (valuesModified) {
		generateSegments();
		valuesModified = false;
	}
}

void Lsystem::setupAlgaeSystem()
{ //Setup the parameters of the original Lindenmayer system
	this->constants.clear();
	this->variables.clear();
	addVariable('A', "AB");
	addVariable('B', "A");
	setAxiom("A");
	setRecursions(7);
}

void Lsystem::setupFractalTreeSystem()
{ //Setup the parameters of a fractal (binary) tree
	this->constants.clear();
	this->variables.clear();
	addVariable('0', "1[0]0", true);
	addVariable('1', "11", true);
	addConstant('[', Rule::PUSH_AND_LEFT);
	addConstant(']', Rule::POP_AND_RIGHT);
	setRecursions(3);
	setAxiom("0");
}

void Lsystem::setupKochCurveSystem()
{ //Setup the parameters for a koch curve using right angles
	this->constants.clear();
	this->variables.clear();
	addVariable('F', "F+F-F-F+F", true);
	addConstant('+', Rule::TURN_LEFT);
	addConstant('-', Rule::TURN_RIGHT);
	setAngleDegrees(90);
	setAxiom("F");
	setRecursions(3);
}

void Lsystem::setupSierpinskiTriangleSystem()
{ //Setup the parameters for a Sierpinski triangle
	this->constants.clear();
	this->variables.clear();
	addVariable('F', "F-G+F+G-F", true);
	addVariable('G', "GG", true);
	addConstant('+', Rule::TURN_LEFT);
	addConstant('-', Rule::TURN_RIGHT);
	setAngleDegrees(120);
	setAxiom("F-G-G");
	setRecursions(6);
}

void Lsystem::setupSierpinskiArrowheadSystem()
{ //Setup the parameters for a Sierpinski arrowhead
	this->constants.clear();
	this->variables.clear();
	addVariable('A', "B-A-B", true);
	addVariable('B', "A+B+A", true);
	addConstant('+', Rule::TURN_LEFT);
	addConstant('-', Rule::TURN_RIGHT);
	setAngleDegrees(60);
	setAxiom("A");
	setRecursions(8);
}

void Lsystem::setupDragonCurveSystem()
{ //Setup the parameters for a Dragon Curve
	this->constants.clear();
	this->variables.clear();
	addVariable('X', "X+YF+");
	addVariable('Y', "-FX-Y");
	addConstant('F', Rule::NONE, true);
	addConstant('-', Rule::TURN_LEFT);
	addConstant('+', Rule::TURN_RIGHT);
	setAngleDegrees(90);
	setAxiom("FX");
	setRecursions(10);
}

void Lsystem::setupFractalPlantSystem()
{ //Setup the parameters for a Fractal plant or 'Barnsley Fern'
	this->constants.clear();
	this->variables.clear();
	addVariable('X', "F+[[x]-X]-F[-FX]+X");
	addVariable('F', "FF", true);
	addConstant('+', Rule::TURN_RIGHT);
	addConstant('-', Rule::TURN_LEFT);
	addConstant('[', Rule::PUSH_POS);
	addConstant(']', Rule::POP_POS);
	setAngleDegrees(25);
	setAxiom("X");
	setRecursions(6);
}

void Lsystem::clearSegments()
{
	this->resultingLSystem.clear();
}

void Lsystem::generateSegments()
{
	clearSegments();

	Vector3 forwardDir = this->forwardDirection * this->forWardDistance;
	int counter = 0;
	float rotX;
	float rotZ;
	char toCompare;
	bool drew = false;
	Vector3 fromPoint = this->startPosition;
	LSegment temp;

	while (this->result[counter]) {
		drew = false;
		toCompare = this->result[counter];
		for (int i = 0; i < this->variables.size() && !drew; i++) {
			if (this->variables[i].character == toCompare) {
				if (this->variables[i].forward) {
					temp.from = fromPoint;
					fromPoint = temp.towards = temp.from + forwardDir;
					this->resultingLSystem.push_back(temp);
					drew = true;
				}
			}
		}
		for (int i = 0; i < this->constants.size() && !drew; i++) {
			if (this->constants[i].character == toCompare) {
				if (this->constants[i].forward) {
					temp.from = fromPoint;
					fromPoint = temp.towards = temp.from + forwardDir;
					this->resultingLSystem.push_back(temp);
					drew = true;
				}
				else if (this->constants[i].action == Rule::TURN_LEFT) {
					rotX = (forwardDir.x * float(cos(this->angle))) - (forwardDir.z * float(sin(this->angle)));
					rotZ = (forwardDir.x * float(sin(this->angle))) + (forwardDir.z * float(cos(this->angle)));
					forwardDir.x = rotX;
					forwardDir.z = rotZ;
					drew = true;
				}
				else if (this->constants[i].action == Rule::TURN_RIGHT) {
					rotX = (forwardDir.x * float((cos(this->angle)))) + (forwardDir.z * float(sin(this->angle)));
					rotZ = (forwardDir.z * float(cos(this->angle))) - (forwardDir.x * float(sin(this->angle)));
					forwardDir.x = rotX;
					forwardDir.z = rotZ;
					drew = true;
				}
				else if (this->constants[i].action == Rule::PUSH_POS) {
					Stored newStored;
					newStored.position = fromPoint;
					newStored.forwardDirection = forwardDir;
					storedPositions.push_back(newStored);
					drew = true;
				}
				else if (this->constants[i].action == Rule::POP_POS) {
					if (storedPositions.size() > 0) {
						fromPoint = storedPositions.back().position;
						forwardDir = storedPositions.back().forwardDirection;
						storedPositions.pop_back();
					}
					drew = true;
				}
				else if (this->constants[i].action == Rule::PUSH_AND_LEFT) {
					Stored newStored;
					newStored.position = fromPoint;
					newStored.forwardDirection = forwardDir;
					storedPositions.push_back(newStored);
					rotX = (forwardDir.x * float(cos(this->angle))) - (forwardDir.z * float(sin(this->angle)));
					rotZ = (forwardDir.x * float(sin(this->angle))) + (forwardDir.z * float(cos(this->angle)));
					forwardDir.x = rotX;
					forwardDir.z = rotZ;
					drew = true;
				}
				else if (this->constants[i].action == Rule::POP_AND_LEFT) {
					if (storedPositions.size() > 0) {
						fromPoint = storedPositions.back().position;
						forwardDir = storedPositions.back().forwardDirection;
						storedPositions.pop_back();
					}
					rotX = (forwardDir.x * float(cos(this->angle))) - (forwardDir.z * float(sin(this->angle)));
					rotZ = (forwardDir.x * float(sin(this->angle))) + (forwardDir.z * float(cos(this->angle)));
					forwardDir.x = rotX;
					forwardDir.z = rotZ;
					drew = true;
				}
				else if (this->constants[i].action == Rule::PUSH_AND_RIGHT) {
					Stored newStored;
					newStored.position = fromPoint;
					newStored.forwardDirection = forwardDir;
					storedPositions.push_back(newStored);
					rotX = (forwardDir.x * float((cos(this->angle)))) + (forwardDir.z * float(sin(this->angle)));
					rotZ = (forwardDir.z * float(cos(this->angle))) - (forwardDir.x * float(sin(this->angle)));
					forwardDir.x = rotX;
					forwardDir.z = rotZ;
					drew = true;
				}
				else if (this->constants[i].action == Rule::POP_AND_RIGHT) {
					if (storedPositions.size() > 0) {
						fromPoint = storedPositions.back().position;
						forwardDir = storedPositions.back().forwardDirection;
						storedPositions.pop_back();
					}
					rotX = (forwardDir.x * float((cos(this->angle)))) + (forwardDir.z * float(sin(this->angle)));
					rotZ = (forwardDir.z * float(cos(this->angle))) - (forwardDir.x * float(sin(this->angle)));
					forwardDir.x = rotX;
					forwardDir.z = rotZ;
					drew = true;
				}
			}
		}
		counter++;
	}

}

String Lsystem::applyRules(String axiom, int recursions)
{
	String toModify = axiom;
	String resulting;
	int counter = 0;
	char toCompare;
	bool added = false;
	if (recursions > 0) {
		while (toModify[counter]) {
			added = false;
			toCompare = toModify[counter];
			counter++;
			for (int i = 0; i < variables.size() && !added; i++) {
				if (variables[i].character == toCompare) {
					resulting += variables[i].replacement;
					added = true;
				}
			}
			if (!added) {
				resulting += toCompare;
			}
		}
		return applyRules(resulting, recursions - 1);
	}
	else {
		resulting = toModify;
		return resulting;
	}
}

void Lsystem::moveLSystem(Vector3 difference)
{
	for (int i = 0; i < this->resultingLSystem.size(); i++) {
		this->resultingLSystem[i].from += difference;
		this->resultingLSystem[i].towards += difference;
	}
}
