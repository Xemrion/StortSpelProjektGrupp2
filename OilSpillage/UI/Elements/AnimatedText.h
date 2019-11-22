#ifndef ANIMATED_TEXT_H
#define ANIMATED_TEXT_H

#include "Element.h"

class AnimatedText : public Element
{
private:
	class TextAnimation
	{
	private:
		std::string& text;
		float& scale;
	public:
		TextAnimation(std::string& text, float& scale);
		virtual ~TextAnimation();

		void update(float deltaTime);
		virtual void draw(bool selected);
	};

	std::string text;
	float scale;
	std::unique_ptr<TextAnimation> animation;
public:
	static Vector2 size;

	AnimatedText(std::string text, float scale, Vector2 position = Vector2());
	virtual ~AnimatedText();

	void update(float deltaTime);
	void draw(bool selected);
};

#endif // !ANIMATED_TEXT_H
