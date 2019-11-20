#ifndef BUTTON_PROMPT_BAR_H
#define BUTTON_PROMPT_BAR_H

#include  "Element.h"
#include "../../Input.h"
#include "../../Texture.h"

struct Prompt
{
	Keys key;
	std::string text;
	Color textColor;
};

class ButtonPromptBar : public Element
{
private:
	Texture* textures[19];
	Texture* texturesPs[19];
	Texture* texturesKb[19];
	std::unique_ptr<Prompt[]> prompts;
	int length;
	Vector2 size;
public:
	ButtonPromptBar(Prompt prompts[], int length, Vector2 position = Vector2());
	virtual ~ButtonPromptBar();

	void draw(bool selected);
	void setPositon(Vector2 position);
	Vector2 getSize() const;
};

#endif // !BUTTON_PROMPT_BAR_H
