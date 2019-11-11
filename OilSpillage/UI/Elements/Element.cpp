#include "Element.h"

void Element::setNeighbours(Element* left, Element* right, Element* up, Element* down)
{
	this->left = left;
	this->right = right;
	this->up = up;
	this->down = down;
}

Element::Element(Vector2 position, float rotation) : position(Vector2(std::roundf(position.x), std::roundf(position.y))), rotation(rotation), left(nullptr), right(nullptr), up(nullptr), down(nullptr)
{
}

Element::~Element()
{
}

Element* Element::getLeft() const
{
	return this->left;
}

Element* Element::getRight() const
{
	return this->right;
}

Element* Element::getUp() const
{
	return this->up;
}

Element* Element::getDown() const
{
	return this->down;
}
