#ifndef UI_UPGRADING_H
#define UI_UPGRADING_H

#include "../UserInterface.h"
#include "../Elements/ItemSelector.h"

class UIUpgrading : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	std::unique_ptr<ItemSelector> itemSelector;
public:
	UIUpgrading();
	virtual ~UIUpgrading();

	void init();
};

#endif // !UI_UPGRADING_H
