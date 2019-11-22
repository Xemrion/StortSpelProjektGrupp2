#ifndef MINIMAP_H
#define MINIMAP_H

#include "Element.h"
#include "../../Texture.h"

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
	Texture* textureObjectiveMarker;
	Texture* textureEnemyMarker;
	Texture* textureCompass;
	Texture* textureArrow;
	float compassRot;

public:
	static Vector2 size;

	Minimap(float zoom, float fogClearRadius = 10.0f, Vector2 position = Vector2());
	virtual ~Minimap();

	void init();
	void draw(bool selected);
	void update(float deltaTime);

	void resetFog();
	bool hasExplored(Vector3 worldPosition) const;
};

#endif // !MINIMAP_H
