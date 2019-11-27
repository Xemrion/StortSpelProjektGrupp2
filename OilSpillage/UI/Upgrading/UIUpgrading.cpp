#include "UIUpgrading.h"
#include "../../Input.h"
#include "../../game.h"
#include "../../States/UpgradingGameState.h"

void UIUpgrading::updateUI(float deltaTime)
{
	if (!this->hasInitStats)
	{
		this->statBox->update(dynamic_cast<UpgradingGameState*>(Game::getCurrentState())->getVehicle()->getStats());
		this->gadgetSelector->init();
		this->hasInitStats = true;
	}
	if (this->selectingItem)
	{
		if (Input::checkButton(Keys::ACTION_1, States::PRESSED))
		{
			Game::setState(Game::STATE_PLAYING);
		}
		else if (Input::checkButton(Keys::CONFIRM, States::PRESSED) && this->itemSelector->isSelectedValid())
		{
			this->selectingItem = false;
		}
		else if (Input::checkButton(Keys::CANCEL, States::PRESSED) && this->itemSelector->isSelectedValid())
		{
			//Remove from car
		}
		else if (Input::checkButton(Keys::L_LEFT, States::PRESSED))
		{
			this->itemSelector->changeSelectedIndex(false);
			Sound::play("./data/sound/MenuMove.wav");
		}
		else if (Input::checkButton(Keys::L_RIGHT, States::PRESSED))
		{
			this->itemSelector->changeSelectedIndex(true);
			Sound::play("./data/sound/MenuMove.wav");
		}
		else if (Input::checkButton(Keys::L_UP, States::PRESSED))
		{
			this->itemSelector->changeSelectedType(false);
			Sound::play("./data/sound/MenuMove.wav");
		}
		else if (Input::checkButton(Keys::L_DOWN, States::PRESSED))
		{
			this->itemSelector->changeSelectedType(true);
			Sound::play("./data/sound/MenuMove.wav");
		}
	}
	else
	{
		this->gadgetSelector->update(true, deltaTime);

		if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
		{
			this->gadgetSelector->setSlotOfSelected(this->itemSelector->getSelectedSlot());
			this->selectingItem = true;
		}
		else if (Input::checkButton(Keys::CANCEL, States::PRESSED))
		{
			this->selectingItem = true;
		}
	}

	this->itemSelector->update(deltaTime);
}

void UIUpgrading::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
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

	UserInterface::getSpriteBatch()->Draw(this->textureSelectorTitleBG->getShaderResView(), this->itemSelector->getPosition() + Vector2(-250.0f, 0.0f));
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), type, Vector2(SCREEN_WIDTH / 2 - ItemSelector::size.x / 2 - 180.0f, SCREEN_HEIGHT - ItemSelector::size.y + 65.0f - 20.0f), Colors::White, 0, Vector2::Zero, 0.5f);
	this->itemSelector->draw(false);
	this->selectingItem ? this->promptBar->draw(false) : this->promptBarGadget->draw(false);

	Vector2 textSize = UserInterface::getFontArial()->MeasureString("Upgrading");
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Upgrading", Vector2(SCREEN_WIDTH / 2 - textSize.x / 2, 0), Colors::White);
	UserInterface::getSpriteBatch()->End();
}

UIUpgrading::UIUpgrading() : textureSelectorTitleBG(nullptr), selectingItem(true)
{
	this->hasInitStats = false;
}

UIUpgrading::~UIUpgrading()
{
}

void UIUpgrading::init()
{
	Game::getGraphics().loadTexture("UI/itemSelectorTitleBG");
	this->textureSelectorTitleBG = Game::getGraphics().getTexturePointer("UI/itemSelectorTitleBG");
	assert(textureSelectorTitleBG && "Texture failed to load!");

	this->itemSelector = std::make_unique<ItemSelector>(Vector2(SCREEN_WIDTH / 2 - ItemSelector::size.x / 2, SCREEN_HEIGHT - ItemSelector::size.y - 20.0f));
	this->gadgetSelector = std::make_unique<CarGadgetSelector>(Vector2::Zero);
	this->statBox = std::make_unique<VehicleStatBox>(Vector2(SCREEN_WIDTH - VehicleStatBox::size.x, 0));

	this->itemSelector->setUsed(this->gadgetSelector->getUsed());

	Prompt prompts[] = {
		{ Keys::L_PRESS, "Move", Color(Colors::White) },
		{ Keys::CONFIRM, "Select Item", Color(Colors::White) },
		{ Keys::CANCEL, "Remove From Car", Color(Colors::White) },
		{ Keys::R_PRESS, "Rotate Car", Color(Colors::White) },
		{ Keys::ACTION_1, "Confirm Upgrading", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 5);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));

	Prompt promptsGadget[] = {
		{ Keys::L_PRESS, "Move", Color(Colors::White) },
		{ Keys::CONFIRM, "Add To Car", Color(Colors::White) },
		{ Keys::CANCEL, "Cancel/Back", Color(Colors::White) },
		{ Keys::R_PRESS, "Rotate Car", Color(Colors::White) },
	};

	this->promptBarGadget = std::make_unique<ButtonPromptBar>(promptsGadget, 4);
	this->promptBarGadget->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBarGadget->getSize().x / 2, SCREEN_HEIGHT - this->promptBarGadget->getSize().y - 8.0f));
}
