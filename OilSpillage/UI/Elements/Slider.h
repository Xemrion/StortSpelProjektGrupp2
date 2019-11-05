#ifndef SLIDER_H
#define SLIDER_H

#include "Element.h"
#include "../../Texture.h"

class Slider : public Element
{
protected:
	Texture* textureBG;
	Texture* textureFG;
	float amount;
public:
	static Vector2 size;

	Slider(Vector2 position = Vector2());
	virtual ~Slider();

	virtual void draw(bool selected);

	void setAmount(float amount);
	float getAmount() const;
};

#endif // !SLIDER_H
