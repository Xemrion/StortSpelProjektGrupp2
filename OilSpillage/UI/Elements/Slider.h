#ifndef SLIDER_H
#define SLIDER_H

#include "Element.h"
#include "../../Texture.h"
#include "../UserInterface.h"

class Slider : public Element
{
protected:
	Texture* textureBG;
	Texture* textureFG;
	float amount;
	DirectX::XMVECTOR color;
public:
	static Vector2 size;

	Slider(Vector2 position = Vector2(), DirectX::XMVECTORF32 color = Colors::Green);
	virtual ~Slider();

	virtual void draw(bool selected);


	void setAmount(float amount);
	float getAmount() const;
};

#endif // !SLIDER_H
