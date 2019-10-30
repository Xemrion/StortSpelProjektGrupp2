#ifndef UI_PLAYING_H
#define UI_PLAYING_H

#include "UserInterface.h"
#include "Slider.h"
#include "Minimap.h"
#include "ObjectiveBox.h"

class UIPlaying : public UserInterface
{
private:
	std::string getFormattedTime();
	void updateUI(float deltaTime);
	void drawUI();

	bool shouldInit;
	float respawnTimer;

	std::unique_ptr<Slider> healthBar;
	std::unique_ptr<Minimap> minimap;
	std::unique_ptr<ObjectiveBox> objectiveBox;
public:
	UIPlaying();
	virtual ~UIPlaying();

	void init();
	void resetMinimapFog();
};

#endif // !UI_PLAYING_H
