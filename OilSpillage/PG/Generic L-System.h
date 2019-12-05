#pragma once
#include"defs.hpp"
#include<d3d11.h>
#include<SimpleMath.h>
using namespace DirectX::SimpleMath;

enum Rule {
	NONE,
	TURN_LEFT,
	TURN_RIGHT,
	PUSH_POS,
	POP_POS,
	PUSH_AND_LEFT,
	PUSH_AND_RIGHT,
	POP_AND_LEFT,
	POP_AND_RIGHT
};
struct LSegment {
	Vector3 from, towards;
};
struct Constant {
	char character = ' ';
	Rule action = NONE;
	bool forward = false;
};
struct Variable {
	char character = ' ';
	String replacement = "";
	bool forward = false;
};
struct Stored {
	Vector3 position;
	Vector3 forwardDirection;
};

class Lsystem
{
public:
	Lsystem();
	~Lsystem();

	void addConstant(char character, Rule action, bool forward = false);
	void removeConstant(char character);
	void addVariable(char character, String replacementString, bool forward = false);
	void removeVariable(char character);
	void setAngleDegrees(float degree);
	void setAngleRadians(float radians);
	void setRecursions(int recursions);
	void setStartPosition(Vector3 startPos);
	void setForwardDirection(Vector3 direction); 
	void setForwardDistance(float distance);
	void setAxiom(String axiom);
	void updateLSystem();

	void setupAlgaeSystem();
	void setupFractalTreeSystem();
	void setupKochCurveSystem();
	void setupSierpinskiTriangleSystem();
	void setupSierpinskiArrowheadSystem();
	void setupDragonCurveSystem();
	void setupFractalPlantSystem();
	Vector<LSegment>& getResultingLSystem();
private:
	Vector<Constant> constants;
	Vector<Variable> variables;
	Vector<LSegment> resultingLSystem;
	Vector<Stored> storedPositions; //Push to, pop from.

	String		axiom;
	String		result;
	Vector3		startPosition;
	Vector3		forwardDirection;
	float		forWardDistance;
	float		angle;
	int			recursions;

	bool rulesModified;
	bool valuesModified;

	void clearSegments();
	void generateSegments();
	String applyRules(String axiom, int recursions);
	void moveLSystem(Vector3 difference);
};
