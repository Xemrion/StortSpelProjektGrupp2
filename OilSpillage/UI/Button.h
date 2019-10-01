#ifndef BUTTON_H
#define BUTTON_H

#include "Element.h"
#include "..//Texture.h"

enum TextAlignment
{
	TopLeft,
	TopMiddle,
	TopRight,
	MiddleLeft,
	Center,
	MiddleRight,
	BottomLeft,
	BottomMiddle,
	BottomRight
};

struct Text
{
	std::string text;
	Color colour = Color(0, 0, 0);
	TextAlignment alignment = TopLeft;

	Text(std::string text, Color colour = Color(0, 0, 0), TextAlignment alignment = TopLeft) : text(text), colour(colour), alignment(alignment) { }
};

class Button : public Element
{
private:
	static Vector2 size;
	Texture* texture;

	Text text;

public:
	Button(Text text, Vector2 position = Vector2());
	virtual ~Button();

	virtual void draw();
};

#endif // !BUTTON_H
