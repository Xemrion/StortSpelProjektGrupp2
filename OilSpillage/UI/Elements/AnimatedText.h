#ifndef ANIMATED_TEXT_H
#define ANIMATED_TEXT_H

#include "Element.h"
#include <optional>

enum class Animation
{
	SHAKING,
	FADE_IN,
	SHAKING_FADE_IN,
	SHAKING_FADE_IN_STOP,
	SPINNING,
	SPINNING_FADE_IN,
	UP_DOWN,
	UP_DOWN_FADE_IN
};

class AnimatedText : public Element
{
private:
	#pragma region Animations
	static constexpr float FADE_SPEED = 0.1f;

	class TextAnimation
	{
	protected:
		Vector2* offsets;
		std::string text;
		Color color;
		float scale;
	public:
		TextAnimation(const std::string& text, const Color& color, float scale);
		virtual ~TextAnimation();

		virtual void update(float deltaTime) = 0;
		virtual void draw(const Vector2& position) = 0;
		virtual void begin() {}
		virtual void end() {}

		virtual void setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale);
	};

	class ShakingAnimation : public TextAnimation
	{
	protected:
		Vector2* randomOffsets;
	public:
		ShakingAnimation(const std::string& text, const Color& color, float scale);
		virtual ~ShakingAnimation();

		virtual void update(float deltaTime);
		virtual void draw(const Vector2& position);

		virtual void setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale);
	};

	class FadeAnimation : public TextAnimation
	{
	private:
		float animationTime;
		float alphaTimer;
	public:
		FadeAnimation(const std::string& text, const Color& color, float scale);
		virtual ~FadeAnimation();

		virtual void update(float deltaTime);
		virtual void draw(const Vector2& position);
		virtual void begin();
		virtual void end();

		virtual void setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale);
	};

	class ShakingFadeAnimation : public ShakingAnimation
	{
	protected:
		float animationTime;
		float alphaTimer;
	public:
		ShakingFadeAnimation(const std::string& text, const Color& color, float scale);
		virtual ~ShakingFadeAnimation();

		virtual void update(float deltaTime);
		virtual void draw(const Vector2& position);

		virtual void setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale);
	};

	class ShakingFadeStopAnimation : public ShakingFadeAnimation
	{
	public:
		ShakingFadeStopAnimation(const std::string& text, const Color& color, float scale);
		virtual ~ShakingFadeStopAnimation();

		virtual void update(float deltaTime);
		virtual void draw(const Vector2& position);
		virtual void end();
	};

	class SpinningAnimation : public TextAnimation
	{
	protected:
		float spinTimer;
		Vector2* spinOffsets;
		bool upDownOnly;
	public:
		SpinningAnimation(const std::string& text, const Color& color, float scale, bool upDownOnly);
		virtual ~SpinningAnimation();

		virtual void update(float deltaTime);
		virtual void draw(const Vector2& position);
		virtual void begin();
		virtual void end();

		virtual void setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale);
	};

	class SpinningFadeAnimation : public SpinningAnimation
	{
	protected:
		float animationTime;
		float alphaTimer;
	public:
		SpinningFadeAnimation(const std::string& text, const Color& color, float scale, bool upDownOnly);
		virtual ~SpinningFadeAnimation();

		virtual void update(float deltaTime);
		virtual void draw(const Vector2& position);
		virtual void begin();
		virtual void end();

		virtual void setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale);
	};
	#pragma endregion

	Vector2 size;
	std::unique_ptr<TextAnimation> animation;
	bool animationActive;
public:
	AnimatedText(const std::string& text, const Color& color, float scale, Animation animation = Animation::FADE_IN, Vector2 position = Vector2());
	virtual ~AnimatedText();

	void update(float deltaTime);
	void draw(bool selected);

	void beginAnimation();
	void endAnimation();

	void setVariables(std::optional<std::string> text, std::optional<Color> color, std::optional<float> scale);
	void setPosition(const Vector2& position);
	Vector2 getSize() const;
};

#endif // !ANIMATED_TEXT_H
