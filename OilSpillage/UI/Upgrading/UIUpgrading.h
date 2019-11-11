#ifndef UI_UPGRADING_H
#define UI_UPGRADING_H

#include "../UserInterface.h"
#include "../Elements/ItemSelector.h"
#include"../Vehicle.h"

class UIUpgrading : public UserInterface
{
private:
	void updateUI(float deltaTime);
	void drawUI();

	std::unique_ptr<ItemSelector> itemSelector;
	std::unique_ptr<VehicleSlots> vehicleSlots;
public:
	UIUpgrading();
	virtual ~UIUpgrading();

	VehicleSlots* getVehicleSlots();
	void init();
};

#endif // !UI_UPGRADING_H
