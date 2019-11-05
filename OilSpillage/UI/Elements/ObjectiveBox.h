#ifndef OBJECTIVE_BOX_H
#define OBJECTIVE_BOX_H

#include "Element.h"
#include"../../Objectives/ObjectiveHandler.h"

class ObjectiveBox : public Element
{
private:
	ObjectiveHandler* objHandlerPtr;
	Texture* textureQuest;
	std::vector<std::string> vecToShow;
	std::string rewardInfo;
	Color color;
public:
	static Vector2 size;

	ObjectiveBox(Vector2 position = Vector2());
	virtual ~ObjectiveBox();

	void init();
	void draw(bool selected);
	void update(float deltaTime);
};


#endif // !OBJECTIVE_BOX_H
