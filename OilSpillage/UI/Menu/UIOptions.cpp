#include "UIOptions.h"
#include "../../Input.h"
#include "../../game.h"
#include "../../States/MenuGameState.h"
#include "../../States/PlayingGameState.h"

void UIOptions::updateUI(float deltaTime)
{
	this->selected = this->changeSelected(this->selected);

	if (Input::checkButton(Keys::CONFIRM, States::PRESSED))
	{
		if (this->selected == this->buttonBack.get())
		{
			MenuGameState* state = dynamic_cast<MenuGameState*>(Game::getCurrentState());

			if (state)
			{
				state->setCurrentMenu(MenuGameState::MENU_MAIN);
			}
			else
			{
				PlayingGameState* state2 = dynamic_cast<PlayingGameState*>(Game::getCurrentState());

				if (state2)
				{
					state2->setCurrentMenu(PlayingGameState::MENU_PAUSED);
				}
			}
		}
		else if (this->selected == this->checkBoxDrivingMode.get())
		{
			this->checkBoxDrivingMode->setActivated(!this->checkBoxDrivingMode->isActivated());
			Game::setDrivingMode(this->checkBoxDrivingMode->isActivated());
		}
	}
	else if (Input::checkButton(Keys::CANCEL, States::PRESSED))
	{
		MenuGameState* state = dynamic_cast<MenuGameState*>(Game::getCurrentState());

		if (state)
		{
			state->setCurrentMenu(MenuGameState::MENU_MAIN);
		}
		else
		{
			PlayingGameState* state2 = dynamic_cast<PlayingGameState*>(Game::getCurrentState());

			if (state2)
			{
				state2->setCurrentMenu(PlayingGameState::MENU_PAUSED);
			}
		}
	}

	if (this->selected == this->sliderMaster.get())
	{
		if (Input::checkButton(Keys::L_LEFT, States::PRESSED))
		{
			this->sliderMaster->setAmount(this->sliderMaster->getAmount() - 0.05f);
			Sound::setVolumeMaster(this->sliderMaster->getAmount());
			Sound::play("MenuMove.wav");
		}
		else if (Input::checkButton(Keys::L_RIGHT, States::PRESSED))
		{
			this->sliderMaster->setAmount(this->sliderMaster->getAmount() + 0.05f);
			Sound::setVolumeMaster(this->sliderMaster->getAmount());
			Sound::play("MenuMove.wav");
		}
	}
	else if (this->selected == this->sliderMusic.get())
	{
		if (Input::checkButton(Keys::L_LEFT, States::PRESSED))
		{
			this->sliderMusic->setAmount(this->sliderMusic->getAmount() - 0.05f);
			Sound::setVolumeSoundtrack(this->sliderMusic->getAmount());
			Sound::play("MenuMove.wav");
		}
		else if (Input::checkButton(Keys::L_RIGHT, States::PRESSED))
		{
			this->sliderMusic->setAmount(this->sliderMusic->getAmount() + 0.05f);
			Sound::setVolumeSoundtrack(this->sliderMusic->getAmount());
			Sound::play("MenuMove.wav");
		}
	}
	else if (this->selected == this->sliderEffects.get())
	{
		if (Input::checkButton(Keys::L_LEFT, States::PRESSED))
		{
			this->sliderEffects->setAmount(this->sliderEffects->getAmount() - 0.05f);
			Sound::setVolumeEffects(this->sliderEffects->getAmount());
			Sound::play("MenuMove.wav");
		}
		else if (Input::checkButton(Keys::L_RIGHT, States::PRESSED))
		{
			this->sliderEffects->setAmount(this->sliderEffects->getAmount() + 0.05f);
			Sound::setVolumeEffects(this->sliderEffects->getAmount());
			Sound::play("MenuMove.wav");
		}
	}
}

void UIOptions::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->buttonBack->draw(this->selected == this->buttonBack.get());
	this->checkBoxDrivingMode->draw(this->selected == this->checkBoxDrivingMode.get());
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Master Volume", this->sliderMaster->getPosition() - Vector2(0, Slider::size.y * 2.0f), this->selected == this->sliderMaster.get() ? Colors::White : Colors::Gray, 0.0f, Vector2::Zero, 0.5f);
	this->sliderMaster->draw(this->selected == this->sliderMaster.get());
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Music Volume", this->sliderMusic->getPosition() - Vector2(0, Slider::size.y * 2.0f), this->selected == this->sliderMusic.get() ? Colors::White : Colors::Gray, 0.0f, Vector2::Zero, 0.5f);
	this->sliderMusic->draw(this->selected == this->sliderMusic.get());
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), "Effects Volume", this->sliderEffects->getPosition() - Vector2(0, Slider::size.y * 2.0f), this->selected == this->sliderEffects.get() ? Colors::White : Colors::Gray, 0.0f, Vector2::Zero, 0.5f);
	this->sliderEffects->draw(this->selected == this->sliderEffects.get());
	this->promptBar->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIOptions::UIOptions() : selected(nullptr)
{
}

UIOptions::~UIOptions()
{
}

void UIOptions::init()
{
	this->checkBoxDrivingMode = std::make_unique<CheckBox>("Realistic Driving", Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2 - CheckBox::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 2.75f));
	this->sliderMaster = std::make_unique<Slider>(Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 1.5f + Slider::size.y * 2.0f));
	this->sliderMusic = std::make_unique<Slider>(Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 1.5f + Slider::size.y * 5.0f));
	this->sliderEffects = std::make_unique<Slider>(Vector2(SCREEN_WIDTH / 2 - Slider::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 1.5f + Slider::size.y * 8.0f));
	this->buttonBack = std::make_unique<Button>(Text("Back", Color(Colors::Black), TextAlignment::Center), Vector2(SCREEN_WIDTH / 2 - Button::size.x / 2, SCREEN_HEIGHT / 2 - Button::size.y * 1.5f + Slider::size.y * 10.0f));

	this->checkBoxDrivingMode->setNeighbours(nullptr, nullptr, this->buttonBack.get(), this->sliderMaster.get());
	this->sliderMaster->setNeighbours(nullptr, nullptr, this->checkBoxDrivingMode.get(), this->sliderMusic.get());
	this->sliderMusic->setNeighbours(nullptr, nullptr, this->sliderMaster.get(), this->sliderEffects.get());
	this->sliderEffects->setNeighbours(nullptr, nullptr, this->sliderMusic.get(), this->buttonBack.get());
	this->buttonBack->setNeighbours(nullptr, nullptr, this->sliderEffects.get(), this->checkBoxDrivingMode.get());

	this->checkBoxDrivingMode->setActivated(Game::getDrivingMode());
	this->sliderMaster->setAmount(Sound::getVolumeMaster());
	this->sliderMusic->setAmount(Sound::getVolumeSoundtrack());
	this->sliderEffects->setAmount(Sound::getVolumeEffects());

	this->selected = this->checkBoxDrivingMode.get();

	Prompt prompts[] = {
		{ Keys::L_PRESS, "Move", Color(Colors::White) },
		{ Keys::CONFIRM, "Confirm", Color(Colors::White) },
		{ Keys::CANCEL, "Cancel/Back", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 3);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));
}

void UIOptions::unloadTextures()
{
	this->buttonBack->unloadTextures();
	this->sliderEffects->unloadTextures();
	this->checkBoxDrivingMode->unloadTextures();
}
