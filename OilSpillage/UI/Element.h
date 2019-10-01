#ifndef ELEMENT_H
#define ELEMENT_H

#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Element
{
protected:
	Vector2 position;
	float rotation;

	Element* left;
	Element* right;
	Element* up;
	Element* down;
public:
	Element(Vector2 position = Vector2(), float rotation = 0.0f);
	virtual ~Element();

	virtual void draw(bool selected) = 0;

	void setNeighbours(Element* left, Element* right, Element* up, Element* down);

	Element* getLeft() const;
	Element* getRight() const;
	Element* getUp() const;
	Element* getDown() const;
};

#endif // !ELEMENT_H