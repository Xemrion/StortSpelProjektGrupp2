#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#include <SpriteBatch.h>
#include "Element.h"

class UserInterface
{
protected:
	static SpriteBatch* getSpriteBatch();
private:
	static std::unique_ptr<SpriteBatch> spriteBatch;
public:
	static void initSpriteBatch();

protected:
	void setElements(Element** elements, int nrOfElements);
private:
	void resetShaders();

	Element** elements;
	int nrOfElements;
public:
	UserInterface();
	virtual ~UserInterface();

	virtual void init() = 0;
	void update(float deltaTime);
};

#endif // !USER_INTERFACE_H
