#ifndef UI_PLAYING_H
#define UI_PLAYING_H

#include "UserInterface.h"
#include "Slider.h"
#include "Minimap.h"

class UIPlaying : public UserInterface
{
private:
	std::string getFormattedTime();
	void updateUI(float deltaTime);
	void drawUI();

	bool initMinimap;
	std::unique_ptr<Slider> healthBar;
	std::unique_ptr<Minimap> minimap;
public:
	UIPlaying();
	virtual ~UIPlaying();

	void init();
};

#endif // !UI_PLAYING_H
