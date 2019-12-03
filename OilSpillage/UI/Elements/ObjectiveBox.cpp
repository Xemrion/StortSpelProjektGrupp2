#include "ObjectiveBox.h"
#include "../../game.h"
#include "../../States/PlayingGameState.h"

Vector2 ObjectiveBox::size = Vector2(218, 160);

ObjectiveBox::ObjectiveBox(Vector2 position) : Element(position), objHandlerPtr(nullptr)
{
	Game::getGraphics().loadTexture("UI/quest");
	this->textureQuest = Game::getGraphics().getTexturePointer("UI/quest");
	assert(textureQuest && "Texture failed to load!");

	this->objectiveText = std::make_unique<AnimatedText>("Objectives", Color(Colors::Yellow), 0.25f, Animation::NONE);
	this->objectiveText->setPosition(this->position + Vector2(ObjectiveBox::size.x / 2 - this->objectiveText->getSize().x / 2, 16));
	this->objectiveText->beginAnimation();

	this->info = std::make_unique<AnimatedText>("Info", Color(Colors::Yellow), 2.0f, Animation::NONE, Vector2(SCREEN_WIDTH / 2 - ObjectiveBox::size.x / 2, SCREEN_HEIGHT / 2 - ObjectiveBox::size.y / 2) + Vector2(16, 64));
	this->info->beginAnimation();

	this->rewardInfo = std::make_unique<AnimatedText>("Reward", Color(Colors::Yellow), 0.2f, Animation::NONE, this->position + Vector2(16, 96));
	this->rewardInfo->beginAnimation();
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
	UserInterface::getSpriteBatch()->Draw(this->textureQuest->getShaderResView(), this->position);
	this->objectiveText->draw(false);

	if (blinkTimer % 5 == 0 && objectiveTimer < 0.5f)
	{
		this->info->draw(false);
		blinkTimer = 0;
	}
	else if (objectiveTimer > 0.5f)
	{
		this->info->draw(false);
	}
	blinkTimer++;

	if (objHandlerPtr->getObjective(0))
	{
		this->rewardInfo->draw(false);
	}
}

void ObjectiveBox::update(float deltaTime)
{
	if (!this->objHandlerPtr->getObjective(0))
	{
		this->info->setVariables("You did it.\nGet out now!!!", Color(Colors::Yellow), {});
	}
	else
	{
		this->info->setVariables(this->objHandlerPtr->getObjective(0)->getInfo(), Color(Colors::Yellow), {});

		if (this->objHandlerPtr->getObjective(0)->getType() == TypeOfMission::GetToPoint)
		{
			this->rewardInfo->setVariables("Reward: 3 Items", Color(Colors::Yellow), {});
		}
		else
		{
			this->rewardInfo->setVariables("Reward: +" + std::to_string(this->objHandlerPtr->getObjective(0)->getRewardTime()) + " sec", Color(Colors::Yellow), {});
		}
	}
	objectiveTimer += deltaTime;
}

void ObjectiveBox::setPosition(Vector2 position)
{
	this->position = position;
	this->objectiveText->setPosition(this->position + Vector2(ObjectiveBox::size.x / 2 - this->objectiveText->getSize().x / 2, 16));

	this->rewardInfo->setPosition(this->position + Vector2(16, 96));
}

void ObjectiveBox::setInfoPosition(Vector2 position, float scale)
{
	this->info->setPosition(position);
	this->info->setVariables({}, {}, scale);
}

Vector2 ObjectiveBox::getInfoSize() const
{
	return info->getSize();
}