#include "ButtonPromtBar.h"
#include "../../game.h"
#include "../UserInterface.h"

ButtonPromptBar::ButtonPromptBar(Prompt prompts[], int length, Vector2 position) : Element(position), length(length)
{ 
	Game::getGraphics().loadTexture("UI/stick_left");
	Game::getGraphics().loadTexture("UI/stick_right");
	Game::getGraphics().loadTexture("UI/button_confirm");
	Game::getGraphics().loadTexture("UI/button_cancel");
	Game::getGraphics().loadTexture("UI/button_menu");
	Game::getGraphics().loadTexture("UI/shoulder_left");
	Game::getGraphics().loadTexture("UI/shoulder_right");
	Game::getGraphics().loadTexture("UI/stick_ps_left");
	Game::getGraphics().loadTexture("UI/stick_ps_right");
	Game::getGraphics().loadTexture("UI/button_ps_confirm");
	Game::getGraphics().loadTexture("UI/button_ps_cancel");
	Game::getGraphics().loadTexture("UI/button_ps_menu");
	Game::getGraphics().loadTexture("UI/shoulder_ps_left");
	Game::getGraphics().loadTexture("UI/shoulder_ps_right");
	Game::getGraphics().loadTexture("UI/stick_kb_left");
	Game::getGraphics().loadTexture("UI/stick_kb_right");
	Game::getGraphics().loadTexture("UI/button_kb_confirm");
	Game::getGraphics().loadTexture("UI/button_kb_cancel");
	Game::getGraphics().loadTexture("UI/button_kb_menu");
	Game::getGraphics().loadTexture("UI/shoulder_kb_left");
	Game::getGraphics().loadTexture("UI/shoulder_kb_right");

	this->textures[static_cast<int>(Keys::L_PRESS)] = Game::getGraphics().getTexturePointer("UI/stick_left");
	this->textures[static_cast<int>(Keys::R_PRESS)] = Game::getGraphics().getTexturePointer("UI/stick_right");
	this->textures[static_cast<int>(Keys::CONFIRM)] = Game::getGraphics().getTexturePointer("UI/button_confirm");
	this->textures[static_cast<int>(Keys::CANCEL)] = Game::getGraphics().getTexturePointer("UI/button_cancel");
	this->textures[static_cast<int>(Keys::MENU)] = Game::getGraphics().getTexturePointer("UI/button_menu");
	this->textures[static_cast<int>(Keys::L_SHOULDER)] = Game::getGraphics().getTexturePointer("UI/shoulder_left");
	this->textures[static_cast<int>(Keys::R_SHOULDER)] = Game::getGraphics().getTexturePointer("UI/shoulder_right");
	this->texturesPs[static_cast<int>(Keys::L_PRESS)] = Game::getGraphics().getTexturePointer("UI/stick_ps_left");
	this->texturesPs[static_cast<int>(Keys::R_PRESS)] = Game::getGraphics().getTexturePointer("UI/stick_ps_right");
	this->texturesPs[static_cast<int>(Keys::CONFIRM)] = Game::getGraphics().getTexturePointer("UI/button_ps_confirm");
	this->texturesPs[static_cast<int>(Keys::CANCEL)] = Game::getGraphics().getTexturePointer("UI/button_ps_cancel");
	this->texturesPs[static_cast<int>(Keys::MENU)] = Game::getGraphics().getTexturePointer("UI/button_ps_menu");
	this->texturesPs[static_cast<int>(Keys::L_SHOULDER)] = Game::getGraphics().getTexturePointer("UI/shoulder_ps_left");
	this->texturesPs[static_cast<int>(Keys::R_SHOULDER)] = Game::getGraphics().getTexturePointer("UI/shoulder_ps_right");
	this->texturesKb[static_cast<int>(Keys::L_PRESS)] = Game::getGraphics().getTexturePointer("UI/stick_kb_left");
	this->texturesKb[static_cast<int>(Keys::R_PRESS)] = Game::getGraphics().getTexturePointer("UI/stick_kb_right");
	this->texturesKb[static_cast<int>(Keys::CONFIRM)] = Game::getGraphics().getTexturePointer("UI/button_kb_confirm");
	this->texturesKb[static_cast<int>(Keys::CANCEL)] = Game::getGraphics().getTexturePointer("UI/button_kb_cancel");
	this->texturesKb[static_cast<int>(Keys::MENU)] = Game::getGraphics().getTexturePointer("UI/button_kb_menu");
	this->texturesKb[static_cast<int>(Keys::L_SHOULDER)] = Game::getGraphics().getTexturePointer("UI/shoulder_kb_left");
	this->texturesKb[static_cast<int>(Keys::R_SHOULDER)] = Game::getGraphics().getTexturePointer("UI/shoulder_kb_right");

	this->prompts = std::make_unique<Prompt[]>(length);
	float sizeX = 0.0f;

	for (int i = 0; i < length; i++)
	{
		this->prompts[i] = prompts[i];

		if (this->textures[static_cast<int>(this->prompts[i].key)])
		{
			sizeX += this->textures[static_cast<int>(this->prompts[i].key)]->getWidth() + UserInterface::getFontArial()->MeasureString(this->prompts[i].text.c_str()).m128_f32[0] * 0.2f + 5.0f;
		}
	}

	sizeX -= 5.0f;
	this->size = Vector2(sizeX, 32.0f);
}

ButtonPromptBar::~ButtonPromptBar()
{
}

void ButtonPromptBar::draw(bool selected)
{
	SpriteBatch* sb = UserInterface::getSpriteBatch();
	SpriteFont* font = UserInterface::getFontArial();
	Texture* texture = nullptr;
	float offset = 0.0f;
	Controllers controller = Input::getControllerID();

	for (int i = 0; i < this->length; i++)
	{
		switch (controller)
		{
		case Controllers::NONE:
			texture = this->texturesKb[static_cast<int>(this->prompts[i].key)];
			break;
		case Controllers::XBOX:
			texture = this->textures[static_cast<int>(this->prompts[i].key)];
			break;
		case Controllers::PLAYSTATION:
			texture = this->texturesPs[static_cast<int>(this->prompts[i].key)];
			break;
		}

		if (texture == nullptr)
		{
			//This texture doesn't exist or hasn't been loaded!
			continue;
		}

		sb->Draw(texture->getShaderResView(), this->position + Vector2(offset, 0.0f));
		offset += texture->getWidth();

		Vector2 textSize = font->MeasureString(this->prompts[i].text.c_str()) * 0.2f;
		font->DrawString(sb, this->prompts[i].text.c_str(), this->position + Vector2(offset, this->size.y / 2 - textSize.y / 2), this->prompts[i].textColor, 0.0f, Vector2::Zero, 0.2f);
		offset += textSize.x;

		offset += 20.0f;
	}
}

void ButtonPromptBar::setPositon(Vector2 position)
{
	this->position = Vector2(std::roundf(position.x), std::roundf(position.y));
}

Vector2 ButtonPromptBar::getSize() const
{
	return this->size;
}

void ButtonPromptBar::unloadTextures()
{
	Game::getGraphics().unloadTexture("UI/stick_left");
	Game::getGraphics().unloadTexture("UI/stick_right");
	Game::getGraphics().unloadTexture("UI/button_confirm");
	Game::getGraphics().unloadTexture("UI/button_cancel");
	Game::getGraphics().unloadTexture("UI/button_action_1");
	Game::getGraphics().unloadTexture("UI/button_action_2");
	Game::getGraphics().unloadTexture("UI/button_menu");
	Game::getGraphics().unloadTexture("UI/stick_ps_left");
	Game::getGraphics().unloadTexture("UI/stick_ps_right");
	Game::getGraphics().unloadTexture("UI/button_ps_confirm");
	Game::getGraphics().unloadTexture("UI/button_ps_cancel");
	Game::getGraphics().unloadTexture("UI/button_ps_action_1");
	Game::getGraphics().unloadTexture("UI/button_ps_action_2");
	Game::getGraphics().unloadTexture("UI/button_ps_menu");
	Game::getGraphics().unloadTexture("UI/stick_kb_left");
	Game::getGraphics().unloadTexture("UI/stick_kb_right");
	Game::getGraphics().unloadTexture("UI/button_kb_confirm");
	Game::getGraphics().unloadTexture("UI/button_kb_cancel");
	Game::getGraphics().unloadTexture("UI/button_kb_action_1");
	Game::getGraphics().unloadTexture("UI/button_kb_action_2");
	Game::getGraphics().unloadTexture("UI/button_kb_menu");
}
