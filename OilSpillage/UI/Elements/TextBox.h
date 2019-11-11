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
	std::string text;
	Color textColor;
	float textScale;
	Vector2 size;
public:
	TextBox(std::string text, Color textColor, Vector2 position = Vector2(), ArrowPlacement arrowPlacement = ArrowPlacement::NONE, float textScale = 0.2f);
	TextBox(std::string text, Color textColor, Vector2 size, Vector2 position = Vector2(), ArrowPlacement arrowPlacement = ArrowPlacement::NONE, float textScale = 0.2f);
	virtual ~TextBox();

	void draw(bool selected);

	void setPosition(Vector2 position);
	Vector2 getSize() const;
};

#endif // !TEXT_BOX_H
