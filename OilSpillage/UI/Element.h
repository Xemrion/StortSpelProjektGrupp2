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

public:
	Element(Vector2 position = Vector2(), float rotation = 0.0f);
	virtual ~Element();

	virtual void draw() = 0;
};

#endif // !ELEMENT_H