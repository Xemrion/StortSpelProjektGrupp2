#ifndef UI_HIGHSCORE_H
#define UI_HIGHSCORE_H

#include "../UserInterface.h"
#include "../Elements/AnimatedText.h"
#include "../Elements/ButtonPromtBar.h"
#include <mutex>
#include <thread>

class UIHighscore : public UserInterface
{
private:
	static void loadHighscore(UIHighscore* ui, bool send, std::string name);
	void updateUI(float deltaTime);
	void drawUI();

	float timer;
	std::unique_ptr<ButtonPromptBar> promptBar;
	std::unique_ptr<AnimatedText> highscore;
	std::unique_ptr<AnimatedText> loading;
	std::unique_ptr<AnimatedText> noConnection;
	std::unique_ptr<AnimatedText> scoreText[10];

	std::thread sendHighscore;
	std::mutex loadingMutex;
public:
	UIHighscore();
	virtual ~UIHighscore();

	void init() {}
	void init(bool send, std::string name);

	virtual void unloadTextures();
};

#endif // !UI_HIGHSCORE_H
