#ifndef UI_UPGRADING_H
#define UI_UPGRADING_H

#include "../UserInterface.h"
#include "../Elements/ItemSelector.h"
#include "../Elements/CarGadgetSelector.h"
#include "../Elements/VehicleStatBox.h"
#include"../..//Vehicle.h"

class UIUpgrading : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	bool selectingItem;
	Texture* textureSelectorTitleBG;
	std::unique_ptr<ItemSelector> itemSelector;
	std::unique_ptr<CarGadgetSelector> gadgetSelector;
	std::unique_ptr<VehicleStatBox> statBox;
public:
	UIUpgrading();
	virtual ~UIUpgrading();

	void init();
};

#endif // !UI_UPGRADING_H
