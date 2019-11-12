#include "UIUpgrading.h"
#include "../../Input.h"
#include "../../game.h"

void UIUpgrading::updateUI(float deltaTime)
{
	if (this->selectingItem)
	{
		if (Input::checkButton(Keys::CONFIRM, States::PRESSED) && this->itemSelector->isSelectedValid())
		{
			this->selectingItem = false;
		}
		else if (Input::checkButton(Keys::L_LEFT, States::PRESSED))
		{
			this->itemSelector->changeSelectedIndex(false);
			Sound::PlaySoundEffect(L"data/sound/MenuMove.wav");
		}
		else if (Input::checkButton(Keys::L_RIGHT, States::PRESSED))
		{
			this->itemSelector->changeSelectedIndex(true);
			Sound::PlaySoundEffect(L"data/sound/MenuMove.wav");
		}
		else if (Input::checkButton(Keys::L_UP, States::PRESSED))
		{
			this->itemSelector->changeSelectedType(false);
			Sound::PlaySoundEffect(L"data/sound/MenuMove.wav");
		}
		else if (Input::checkButton(Keys::L_DOWN, States::PRESSED))
		{
			this->itemSelector->changeSelectedType(true);
			Sound::PlaySoundEffect(L"data/sound/MenuMove.wav");
		}
	}
	else
	{
		this->gadgetSelector->update(true, deltaTime);

		if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
		{
			this->gadgetSelector->setSlotOfSelected(this->itemSelector->getSelectedSlot());
		}
		else if (Input::checkButton(Keys::CANCEL, States::PRESSED))
		{
			this->selectingItem = true;
		}
	}

	this->itemSelector->update(deltaTime);

	if (Input::checkButton(Keys::MENU, States::PRESSED))
	{
		Game::setState(Game::STATE_PLAYING);
	}
}

void UIUpgrading::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->itemSelector->draw(false);
	this->gadgetSelector->draw(!this->selectingItem);
	this->statBox->draw(!this->selectingItem);

	const char* type = "";
	switch (this->itemSelector->getSelectedType())
	{
	case WEAPON:
		type = "Weapons";
		break;
	case GADGET:
		type = "Gadgets";
		break;
	case CHASSI:
		type = "Chassis";
		break;
	case WHEEL:
		type = "Wheels";
		break;
	}

	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), type, Vector2(SCREEN_WIDTH / 2 - ItemSelector::size.x / 2 - 200.0f, SCREEN_HEIGHT - ItemSelector::size.y + 65.0f), Colors::White, 0, Vector2::Zero, 0.5f);
	UserInterface::getSpriteBatch()->End();
}

UIUpgrading::UIUpgrading() : selectingItem(true)
{
}

UIUpgrading::~UIUpgrading()
{
}

VehicleSlots* UIUpgrading::getVehicleSlots()
{
	return this->vehicleSlots.get();
}

void UIUpgrading::init()
{
	this->vehicleSlots = std::make_unique<VehicleSlots>();
	this->itemSelector = std::make_unique<ItemSelector>(Vector2(SCREEN_WIDTH / 2 - ItemSelector::size.x / 2, SCREEN_HEIGHT - ItemSelector::size.y));
	this->gadgetSelector = std::make_unique<CarGadgetSelector>(Vector2(ItemSlot::size.x + 20.0f, ItemSlot::size.x + 20.0f));
	this->statBox = std::make_unique<VehicleStatBox>(Vector2(SCREEN_WIDTH - VehicleStatBox::size.x - 10.0f, SCREEN_HEIGHT / 2 - VehicleStatBox::size.y / 2));

	this->itemSelector->setUsedSlots(this->gadgetSelector->getUsedSlots());
}
