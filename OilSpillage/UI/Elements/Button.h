#ifndef BUTTON_H
#define BUTTON_H

#include "Element.h"
#include "../../Texture.h"

enum class TextAlignment
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
	TextAlignment alignment = TextAlignment::TopLeft;

	Text(std::string text, Color colour = Color(0, 0, 0), TextAlignment alignment = TextAlignment::TopLeft) : text(text), colour(colour), alignment(alignment) { }
};

class Button : public Element
{
private:
	Texture* texture;
	Text text;
public:
	static Vector2 size;

	Button(Text text, Vector2 position = Vector2());
	virtual ~Button();

	virtual void draw(bool selected);
};

#endif // !BUTTON_H
