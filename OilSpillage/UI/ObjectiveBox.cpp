#include "ObjectiveBox.h"
#include "../game.h"
#include "../States/PlayingGameState.h"

ObjectiveBox::ObjectiveBox(Vector2 position) : Element(position), objHandlerPtr(nullptr)
{
	Game::getGraphics().loadTexture("UI/quest");
	this->textureQuest = Game::getGraphics().getTexturePointer("UI/quest");
	assert(textureQuest && "Texture failed to load!");
}

ObjectiveBox::~ObjectiveBox()
{
}

void ObjectiveBox::init()
{
	this->objHandlerPtr = &static_cast<PlayingGameState*>(Game::getCurrentState())->getObjHandler();
}

void ObjectiveBox::draw(bool selected)
{
	UserInterface::getSpriteBatch()->Draw(this->textureQuest->getShaderResView(), this->position, nullptr, Colors::White, 0.0f, Vector2(0, 0), Vector2(0.5f,0.3f));

	for (int i = 0; i < vecToShow.size(); i++)
	{
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), vecToShow[i].c_str(), this->position + Vector2(80.0f, 50.0f + i * 18.0f), color, 0, Vector2(), 0.2f);
	}

	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), rewardInfo.c_str(), this->position + Vector2(80.0f, 120.0f), color, 0, Vector2(), 0.2f);
}

void ObjectiveBox::update(float deltaTime)
{
	vecToShow.clear();

	std::string infoUI;
	if (objHandlerPtr->getObjective(0) == nullptr)
	{
		infoUI = "You did it. Get out now!!! ";
		color = Colors::Yellow;
		rewardInfo = "";

	}
	else
	{
		infoUI = objHandlerPtr->getObjective(0)->getInfo();
		rewardInfo = "Reward: " + std::to_string(objHandlerPtr->getObjective(0)->getRewardTime()) + " extra seconds ";
		color = Colors::White;
	}

	int foundSpace = -1;
	int wordIndex = 0;
	std::vector<std::string> spacedString;
	for (int i = 0; i < infoUI.size(); i++)
	{
		if (infoUI[i] == ' ')
		{
			foundSpace = i;
			std::string word;
			for (int j = wordIndex; j < foundSpace + 1; j++)
			{
				word += infoUI[j];
			}
			if (wordIndex == foundSpace + 1)
			{
				word += infoUI[wordIndex];
			}
			word += " ";
			wordIndex = foundSpace + 1;
			spacedString.push_back(word);
		}
	}

	int sizeOfStrings = 0;
	std::string temp = "";
	for (int i = 0; i < spacedString.size(); i++)
	{
		sizeOfStrings += spacedString.at(i).size();
		temp += spacedString.at(i);
		if (sizeOfStrings >= 26)
		{
			sizeOfStrings = 0;
			vecToShow.push_back(temp);
			temp = "";
		}
	}
	vecToShow.push_back(temp);
}
