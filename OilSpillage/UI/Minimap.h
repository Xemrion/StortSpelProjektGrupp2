#ifndef MINIMAP_H
#define MINIMAP_H

#include "Element.h"
#include "../Texture.h"

class Minimap : public Element
{
private:
	float zoom;
	Matrix mapMatrix;

	Texture* textureMap;
	Texture* textureOutline;
	Texture* texturePlayerMarker;
public:
	static Vector2 size;

	Minimap(float zoom, Vector2 position = Vector2());
	virtual ~Minimap();

	void init();
	void draw(bool selected);
};

#endif // !MINIMAP_H
