#ifndef OBJECTIVE_BOX_H
#define OBJECTIVE_BOX_H

#include "Element.h"
#include "AnimatedText.h"
#include"../../Objectives/ObjectiveHandler.h"

class ObjectiveBox : public Element
{
private:
	ObjectiveHandler* objHandlerPtr;
	Texture* textureQuest;
	
	std::unique_ptr<AnimatedText> objectiveText;
	std::unique_ptr<AnimatedText> info;
	std::unique_ptr<AnimatedText> rewardInfo;
	int blinkTimer = 1;
	float objectiveTimer;
public:
	static Vector2 size;

	ObjectiveBox(Vector2 position = Vector2());
	virtual ~ObjectiveBox();

	void init();
	void draw(bool selected);
	void update(float deltaTime);
	void setPosition(Vector2 position);
	void setInfoPosition(Vector2 position, float scale);
	Vector2 getInfoSize() const;
};


#endif // !OBJECTIVE_BOX_H
