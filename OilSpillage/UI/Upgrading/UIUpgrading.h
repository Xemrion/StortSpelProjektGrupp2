#ifndef UI_UPGRADING_H
#define UI_UPGRADING_H

#include "../UserInterface.h"
#include "../Elements/ItemSelector.h"
#include "../Elements/CarGadgetSelector.h"

class UIUpgrading : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	bool selectingItem;
	std::unique_ptr<ItemSelector> itemSelector;
	std::unique_ptr<CarGadgetSelector> carGadgetSelector;
public:
	UIUpgrading();
	virtual ~UIUpgrading();

	void init();
};

#endif // !UI_UPGRADING_H
