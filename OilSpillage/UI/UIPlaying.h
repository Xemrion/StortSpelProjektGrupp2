#ifndef UI_PLAYING_H
#define UI_PLAYING_H

#include "UserInterface.h"
#include "Slider.h"

class UIPlaying : public UserInterface
{
private:
	std::string getFormattedTime();
	void updateUI(float deltaTime);
	void drawUI();

	std::unique_ptr<Slider> healthBar;
public:
	UIPlaying();
	virtual ~UIPlaying();

	void init();
};

#endif // !UI_PLAYING_H
