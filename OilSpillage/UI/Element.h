#ifndef ELEMENT_H
#define ELEMENT_H

#include <d3d11.h>
#include <SimpleMath.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class Element
{
private:
	Vector2 position;
	Vector2 rotation;

public:
	Element(Vector2 position = Vector2(), Vector2 rotation = Vector2());
	virtual ~Element();

	virtual void update(float deltaTime);
	virtual void draw();
};

#endif // !ELEMENT_H