#ifndef TEXT_BOX_H
#define TEXT_BOX_H

#include "Element.h"
#include "../../Texture.h"

enum class ArrowPlacement
{
	NONE,
	TOP,
	RIGHT,
	BOTTOM,
	LEFT
};

class TextBox : public Element
{
private:
	Texture* textureArrow;
	Texture* textureCorner;
	Texture* textureSide;
	Texture* textureMiddle;

	ArrowPlacement arrowPlacement;
	const char* text;
	Color textColor;
public:
	Vector2 size;
	TextBox(const char* text, Color textColor, Vector2 size, ArrowPlacement arrowPlacement = ArrowPlacement::NONE, Vector2 position = Vector2());
	virtual ~TextBox();

	void draw(bool selected);
};

#endif // !TEXT_BOX_H
