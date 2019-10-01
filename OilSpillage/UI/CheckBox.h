#ifndef CHECKBOX_H
#define CHECKBOX_H

#include "Element.h"
#include "..//Texture.h"

class CheckBox : public Element
{
private:
	Texture* textureUnchecked;
	Texture* textureChecked;
	bool activated;
public:
	static Vector2 size;

	CheckBox(Vector2 position = Vector2());
	virtual ~CheckBox();

	virtual void draw(bool selected);

	void setActivated(bool activated);
	bool isActivated() const;
};

#endif // !CHECKBOX_H
