#ifndef MINIMAP_H
#define MINIMAP_H

#include "Element.h"
#include "../Texture.h"

class Minimap : public Element
{
private:
	float fogClearRadius;
	float zoom;
	Matrix mapMatrix;

	ID3D11ShaderResourceView* resourceFog;
	ID3D11Texture2D* textureFog;
	unsigned char* pixels;

	Texture* textureMap;
	Texture* textureFogTemp;
	Texture* textureOutline;
	Texture* texturePlayerMarker;
public:
	static Vector2 size;

	Minimap(float zoom, float fogClearRadius = 10.0f, Vector2 position = Vector2());
	virtual ~Minimap();

	void init();
	void draw(bool selected);
	void update(float deltaTime);
};

#endif // !MINIMAP_H
