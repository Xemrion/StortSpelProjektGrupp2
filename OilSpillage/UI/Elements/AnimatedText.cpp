#include "AnimatedText.h"
#include "../UserInterface.h"

AnimatedText::AnimatedText(const std::string& text, const Color& color, float scale, Animation animation, Vector2 position) : Element(position), animationActive(false)
{
	this->size = UserInterface::getFontArial()->MeasureString(text.c_str()) * scale;

	switch (animation)
	{
	case Animation::SHAKING:
		this->animation = std::make_unique<ShakingAnimation>(text, color, scale);
		break;
	case Animation::SHAKING_FADE_IN:
		this->animation = std::make_unique<ShakingFadeAnimation>(text, color, scale);
		break;
	case Animation::SHAKING_FADE_IN_STOP:
		this->animation = std::make_unique<ShakingFadeStopAnimation>(text, color, scale);
		break;
	case Animation::SPINNING:
		this->animation = std::make_unique<SpinningAnimation>(text, color, scale, false);
		break;
	case Animation::SPINNING_FADE_IN:
		this->animation = std::make_unique<SpinningFadeAnimation>(text, color, scale, false);
		break;
	case Animation::UP_DOWN:
		this->animation = std::make_unique<SpinningAnimation>(text, color, scale, true);
		break;
	case Animation::UP_DOWN_FADE_IN:
		this->animation = std::make_unique<SpinningFadeAnimation>(text, color, scale, true);
		break;
	default:
		this->animation = std::make_unique<FadeAnimation>(text, color, scale);
		break;
	}
}

AnimatedText::~AnimatedText()
{
}

void AnimatedText::update(float deltaTime)
{
	if (this->animationActive)
	{
		this->animation->update(deltaTime);
	}
}

void AnimatedText::draw(bool selected)
{
	this->animation->draw(this->position);
}

void AnimatedText::beginAnimation()
{
	this->animationActive = true;
	this->animation->begin();
}

void AnimatedText::endAnimation()
{
	this->animationActive = false;
	this->animation->end();
}

void AnimatedText::setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale)
{
	this->animation->setVariables(text, color, scale);
}

void AnimatedText::setPosition(const Vector2& position)
{
	this->position = position;
}

Vector2 AnimatedText::getSize() const
{
	return this->size;
}

AnimatedText::TextAnimation::TextAnimation(const std::string& text, const Color& color, float scale) : text(text), color(color), scale(scale)
{
	SpriteFont* font = UserInterface::getFontArial();
	const SpriteFont::Glyph* glyph = nullptr;
	this->offsets = new Vector2[text.length()];
	this->offsets[0] = Vector2::Zero;

	for (int i = 1; i < text.length(); i++)
	{
		if (text[static_cast<long long>(i) - 1] == '\n')
		{
			this->offsets[i] = Vector2(0.0f, this->offsets[i - 1].y + font->GetLineSpacing() * this->scale);
		}
		else
		{
			glyph = font->FindGlyph(text[static_cast<long long>(i) - 1]);
			this->offsets[i] = Vector2(this->offsets[i - 1].x + (glyph->Subrect.right - glyph->Subrect.left + glyph->XOffset + glyph->XAdvance) * this->scale, this->offsets[i - 1].y);
		}
	}
}

AnimatedText::TextAnimation::~TextAnimation()
{
	delete[] this->offsets;
}

void AnimatedText::TextAnimation::setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale)
{
	if (color.has_value())
	{
		this->color = color.value();
	}

	if (scale.has_value())
	{
		this->scale = scale.value();
	}

	if (text.has_value())
	{
		delete[] this->offsets;
		this->text = text.value();

		SpriteFont* font = UserInterface::getFontArial();
		const SpriteFont::Glyph* glyph = nullptr;
		this->offsets = new Vector2[text->length()];
		this->offsets[0] = Vector2::Zero;

		for (int i = 1; i < this->text.length(); i++)
		{
			if (this->text[static_cast<long long>(i) - 1] == '\n')
			{
				this->offsets[i] = Vector2(0.0f, this->offsets[i - 1].y + font->GetLineSpacing() * this->scale);
			}
			else
			{
				glyph = font->FindGlyph(this->text[static_cast<long long>(i) - 1]);
				this->offsets[i] = Vector2(this->offsets[i - 1].x + (glyph->Subrect.right - glyph->Subrect.left + glyph->XOffset + glyph->XAdvance) * this->scale, this->offsets[i - 1].y);
			}
		}
	}
}

AnimatedText::ShakingAnimation::ShakingAnimation(const std::string& text, const Color& color, float scale)
	: TextAnimation(text, color, scale)
{
	this->randomOffsets = new Vector2[text.length()];
}

AnimatedText::ShakingAnimation::~ShakingAnimation()
{
	delete[] this->randomOffsets;
}

void AnimatedText::ShakingAnimation::update(float deltaTime)
{
	for (int i = 0; i < text.length(); i++)
	{
		this->randomOffsets[i] = Vector2((rand() % 10) - 5.0f, (rand() % 10) - 5.0f) * this->scale;
	}
}

void AnimatedText::ShakingAnimation::draw(const Vector2& position)
{
	for (int i = 0; i < text.length(); i++)
	{
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), this->text.substr(i, 1).c_str(), position + this->offsets[i] + this->randomOffsets[i], this->color, 0.0f, Vector2::Zero, this->scale);
	}
}

void AnimatedText::ShakingAnimation::setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale)
{
	TextAnimation::setVariables(text, color, scale);

	if (text.has_value())
	{
		delete[] this->randomOffsets;
		this->randomOffsets = new Vector2[text->length()];
	}
}

AnimatedText::FadeAnimation::FadeAnimation(const std::string& text, const Color& color, float scale)
	: TextAnimation(text, color, scale), animationTime(FADE_SPEED * text.length()), alphaTimer(0.0f)
{
}

AnimatedText::FadeAnimation::~FadeAnimation()
{
}

void AnimatedText::FadeAnimation::update(float deltaTime)
{
	if (this->alphaTimer < this->animationTime)
	{
		this->alphaTimer += deltaTime;
	}
}

void AnimatedText::FadeAnimation::draw(const Vector2& position)
{
	float amountDone = min(this->alphaTimer / this->animationTime, 1.0f);
	float amountNeeded = 0.0f;
	float alpha = 0.0f;

	for (int i = 0; i < text.length(); i++)
	{
		amountNeeded = i / static_cast<float>(this->text.length());

		if (amountDone >= amountNeeded)
		{
			alpha = min((amountDone - amountNeeded) / (1 / static_cast<float>(this->text.length())), 1.0f);
		}
		else
		{
			alpha = 0.0f;
		}
		
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), this->text.substr(i, 1).c_str(), position + this->offsets[i], this->color * Color(1, 1, 1, alpha), 0.0f, Vector2::Zero, this->scale);
	}
}

void AnimatedText::FadeAnimation::begin()
{
	this->alphaTimer = 0.0f;
}

void AnimatedText::FadeAnimation::end()
{
	this->alphaTimer = this->animationTime;
}

void AnimatedText::FadeAnimation::setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale)
{
	TextAnimation::setVariables(text, color, scale);

	if (text.has_value())
	{
		this->animationTime = FADE_SPEED * text->length();
	}
}

AnimatedText::ShakingFadeAnimation::ShakingFadeAnimation(const std::string& text, const Color& color, float scale)
	: ShakingAnimation(text, color, scale), animationTime(FADE_SPEED * text.length()), alphaTimer(0.0f)
{
}

AnimatedText::ShakingFadeAnimation::~ShakingFadeAnimation()
{
}

void AnimatedText::ShakingFadeAnimation::update(float deltaTime)
{
	ShakingAnimation::update(deltaTime);

	if (this->alphaTimer < this->animationTime)
	{
		this->alphaTimer += deltaTime;
	}
}

void AnimatedText::ShakingFadeAnimation::draw(const Vector2& position)
{
	float amountDone = min(this->alphaTimer / this->animationTime, 1.0f);
	float amountNeeded = 0.0f;
	float alpha = 0.0f;

	for (int i = 0; i < text.length(); i++)
	{
		amountNeeded = i / static_cast<float>(this->text.length());

		if (amountDone >= amountNeeded)
		{
			alpha = min((amountDone - amountNeeded) / (1 / static_cast<float>(this->text.length())), 1.0f);
		}
		else
		{
			alpha = 0.0f;
		}

		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), this->text.substr(i, 1).c_str(), position + this->offsets[i] + this->randomOffsets[i], this->color * Color(1, 1, 1, alpha), 0.0f, Vector2::Zero, this->scale);
	}
}

void AnimatedText::ShakingFadeAnimation::setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale)
{
	ShakingAnimation::setVariables(text, color, scale);

	if (text.has_value())
	{
		this->animationTime = FADE_SPEED * text->length();
	}
}

AnimatedText::ShakingFadeStopAnimation::ShakingFadeStopAnimation(const std::string& text, const Color& color, float scale)
	: ShakingFadeAnimation(text, color, scale)
{
}

AnimatedText::ShakingFadeStopAnimation::~ShakingFadeStopAnimation()
{
}

void AnimatedText::ShakingFadeStopAnimation::update(float deltaTime)
{
	ShakingAnimation::update(deltaTime);

	if (this->alphaTimer < this->animationTime + FADE_SPEED * 2.0f)
	{
		this->alphaTimer += deltaTime;
	}
}

void AnimatedText::ShakingFadeStopAnimation::draw(const Vector2& position)
{
	float amountDone = this->alphaTimer / this->animationTime;
	float amountNeeded = 0.0f;
	float alpha = 0.0f;

	for (int i = 0; i < text.length(); i++)
	{
		amountNeeded = i / static_cast<float>(this->text.length());

		if (amountDone >= amountNeeded)
		{
			alpha = (amountDone - amountNeeded) / (1 / static_cast<float>(this->text.length()));
		}
		else
		{
			alpha = 0.0f;
		}

		if (!(alpha < 3.0f && alpha > 0.0f))
		{
			this->randomOffsets[i] = Vector2::Zero;
		}

		alpha = min(alpha, 1.0f);

		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), this->text.substr(i, 1).c_str(), position + this->offsets[i] + this->randomOffsets[i], this->color * Color(1, 1, 1, alpha), 0.0f, Vector2::Zero, this->scale);
	}
}

void AnimatedText::ShakingFadeStopAnimation::end()
{
	this->alphaTimer = this->animationTime + FADE_SPEED * 2.0f;
}

AnimatedText::SpinningAnimation::SpinningAnimation(const std::string& text, const Color& color, float scale, bool upDownOnly)
	: TextAnimation(text, color, scale), spinTimer(0.0f), upDownOnly(upDownOnly)
{
	this->spinOffsets = new Vector2[this->text.length()];
}

AnimatedText::SpinningAnimation::~SpinningAnimation()
{
	delete[] this->spinOffsets;
}

void AnimatedText::SpinningAnimation::update(float deltaTime)
{
	this->spinTimer += std::fmodf(deltaTime * 4, XM_2PI);

	for (int i = 0; i < text.length(); i++)
	{
		this->spinOffsets[i] = Vector2(this->upDownOnly ? 0.0f : std::cos(this->spinTimer + (XM_2PI / 24.0f) * i), std::sin(this->spinTimer + (XM_2PI / 24.0f) * i)) * 10.0f * this->scale;
	}
}

void AnimatedText::SpinningAnimation::draw(const Vector2& position)
{
	for (int i = 0; i < text.length(); i++)
	{
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), this->text.substr(i, 1).c_str(), position + this->offsets[i] + this->spinOffsets[i], this->color, 0.0f, Vector2::Zero, this->scale);
	}
}

void AnimatedText::SpinningAnimation::begin()
{
	this->spinTimer = 0.0f;
}

void AnimatedText::SpinningAnimation::end()
{
	this->spinTimer = 0.0f;
}

void AnimatedText::SpinningAnimation::setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale)
{
	TextAnimation::setVariables(text, color, scale);

	delete[] this->spinOffsets;
	this->spinOffsets = new Vector2[this->text.length()];
}

AnimatedText::SpinningFadeAnimation::SpinningFadeAnimation(const std::string& text, const Color& color, float scale, bool upDownOnly)
	: SpinningAnimation(text, color, scale, upDownOnly), animationTime(FADE_SPEED * text.length()), alphaTimer(0.0f)
{
}

AnimatedText::SpinningFadeAnimation::~SpinningFadeAnimation()
{
}

void AnimatedText::SpinningFadeAnimation::update(float deltaTime)
{
	SpinningAnimation::update(deltaTime);

	if (this->alphaTimer < this->animationTime)
	{
		this->alphaTimer += deltaTime;
	}
}

void AnimatedText::SpinningFadeAnimation::draw(const Vector2& position)
{
	float amountDone = min(this->alphaTimer / this->animationTime, 1.0f);
	float amountNeeded = 0.0f;
	float alpha = 0.0f;

	for (int i = 0; i < text.length(); i++)
	{
		amountNeeded = i / static_cast<float>(this->text.length());

		if (amountDone >= amountNeeded)
		{
			alpha = min((amountDone - amountNeeded) / (1 / static_cast<float>(this->text.length())), 1.0f);
		}
		else
		{
			alpha = 0.0f;
		}

		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), this->text.substr(i, 1).c_str(), position + this->offsets[i] + this->spinOffsets[i], this->color * Color(1, 1, 1, alpha), 0.0f, Vector2::Zero, this->scale);
	}
}

void AnimatedText::SpinningFadeAnimation::begin()
{
	SpinningAnimation::begin();
	this->alphaTimer = 0.0f;
}

void AnimatedText::SpinningFadeAnimation::end()
{
	SpinningAnimation::end();
	this->alphaTimer = this->animationTime;
}

void AnimatedText::SpinningFadeAnimation::setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale)
{
	SpinningAnimation::setVariables(text, color, scale);

	if (text.has_value())
	{
		this->animationTime = FADE_SPEED * text->length();
	}
}
