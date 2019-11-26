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
{
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
{
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
{
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
{
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
{
	this->angle = degree * ((3.1415926535f) / 180.0f);
	valuesModified = true;
}

void Lsystem::setAngleRadians(float radians)
{
	this->angle = radians;
	valuesModified = true;
}

void Lsystem::setRecursions(int recursions)
{
	this->recursions = recursions;
	rulesModified = true;
	valuesModified = true;
}

void Lsystem::setStartPosition(Vector3 startPos)
{
	Vector3 difference = startPos - this->startPosition;
	this->startPosition += difference;
	moveLSystem(difference);
}

void Lsystem::setForwardDirection(Vector3 direction)
{
	this->forwardDirection = direction;
	this->forwardDirection.Normalize();
	valuesModified = true;
}

void Lsystem::setForwardDistance(float distance)
{
	this->forWardDistance = distance;
	valuesModified = true;
}

void Lsystem::setAxiom(String axiom)
{
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
			}
		}
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
			toCompare = toModify[counter];
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
