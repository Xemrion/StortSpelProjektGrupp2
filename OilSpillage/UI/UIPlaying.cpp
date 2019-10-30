#include "UIPlaying.h"
#include "..//Input.h"
#include "../game.h"
#include "../States/PlayingGameState.h"
#include <sstream>
std::string UIPlaying::getFormattedTime()
{
	int time = static_cast<int>(static_cast<PlayingGameState*>(Game::getCurrentState())->getTime());
	int minutes = time / 60;
	int seconds = time % 60;

	std::stringstream timeStrStream;

	if (minutes < 10)
		timeStrStream << "0";

	timeStrStream << minutes;
	timeStrStream << ":";

	if (seconds < 10)
		timeStrStream << "0";

	timeStrStream << seconds;

	return timeStrStream.str();
}

void UIPlaying::updateUI(float deltaTime)
{
	if (this->initMinimap)
	{
		this->minimap->init();
		this->initMinimap = false;
	}
	if (this->respawnTimer >= 1.0f)
	{
		this->respawnTimer = 0.0f;
	}
	this->respawnTimer += deltaTime;
	this->minimap->update(deltaTime);
}

void UIPlaying::drawUI()
{
	float time = static_cast<int>(static_cast<PlayingGameState*>(Game::getCurrentState())->getTime());
	this->objHandlerPtr = &static_cast<PlayingGameState*>(Game::getCurrentState())->getObjHandler();
	std::string timeStr = this->getFormattedTime();
	Vector2 textSize = UserInterface::getFontArial()->MeasureString(timeStr.c_str());
	Vector2 timeScale(1.0f, 1.0f);
	Vector2 shake(0, 0);
	if (time < 11)
	{
		shake.x = rand() % 10 + 1;
		shake.y = rand() % 10 + 1;
		shake += Vector2(11 - time);
	}
	Vector2 position(Vector2(SCREEN_WIDTH / 2, textSize.y) + shake);
	Vehicle* player = static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer();

	this->healthBar->setAmount(player->getHealth() / static_cast<float>(player->getMaxHealth()));
	

	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	
	
	Vector4 colorOverTime = Vector4::Lerp(Vector4(Colors::Yellow), Vector4(Colors::Red), (20.0f-time)/20.0f);
	if (time > 20.0f)
	{
		colorOverTime = Colors::Yellow;
	}
	if (time <= 0)
	{
		timeStr = "Game Over Press Enter to Continue";
		textSize = UserInterface::getFontArial()->MeasureString(timeStr.c_str());
		timeScale = Vector2(0.7f, 0.7f);
		position = Vector2(SCREEN_WIDTH / 2 , 720 / 2);
	}
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), timeStr.c_str(), position, colorOverTime, 0, Vector2(textSize.x / 2, textSize.y / 2), timeScale);
	
	std::string rewardInfo;
	Color color;
	std::string infoUI;
	if (objHandlerPtr->getObjective(0)==nullptr)
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
			for (int j = wordIndex; j < foundSpace+1; j++)
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
	std::vector<std::string> vecToShow;
	int sizeOfStrings = 0;
	std::string temp = "";
	for (int i = 0; i < spacedString.size(); i++)
	{
		sizeOfStrings += spacedString.at(i).size();
		temp += spacedString.at(i);
		if (sizeOfStrings >= 20)
		{
			sizeOfStrings = 0;
			vecToShow.push_back(temp);
			temp = "";
		}
	}
	vecToShow.push_back(temp);
	RECT rc = { 0, 0, quest->getWidth(), quest->getHeight() };
	Vector2 questPos(0.0f, 50);
	//void XM_CALLCONV Draw(_In_ ID3D11ShaderResourceView * texture, XMFLOAT2 const& position, _In_opt_ RECT const* sourceRectangle, FXMVECTOR color = Colors::White, float rotation = 0, XMFLOAT2 const& origin = Float2Zero, float scale = 1, SpriteEffects effects = SpriteEffects_None, float layerDepth = 0);
	UserInterface::getSpriteBatch()->Draw(this->quest->getShaderResView(), questPos, &rc, Colors::White, 0.0f, Vector2(0, 0), 0.4f);// , & rc, Vector3(1, 1, 1), 0.0f, Vector2(1, 1), 1.0f);
	
	for (int i = 0; i < vecToShow.size(); i++)
	{
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), vecToShow.at(i).c_str(), Vector2(questPos.x + 60.0f, questPos.y + 60.0f + i*15.0f), color, 0, Vector3(0, 0, 0), Vector3(0.2, 0.2, 0.2));
	}
	UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), rewardInfo.c_str(), Vector2(questPos.x + 60.0f, questPos.y + 120.0f), color, 0, Vector3(0, 0, 0), Vector3(0.2, 0.2, 0.2));
	std::string respawnTime = std::to_string(int(player->getTotRespawnTime() - player->getRespawnTimer()));
	if (player->getRespawnTimer() > 0)
	{
		Vector2 textSize = UserInterface::getFontArial()->MeasureString(respawnTime.c_str());
		float sizeOverT = 0.1f + ((1 - this->respawnTimer) / 1)* (1.0f - 0.1f);
		Vector3 sizeLerp(sizeOverT);
		UserInterface::getFontArial()->DrawString(UserInterface::getSpriteBatch(), respawnTime.c_str(), Vector2( (SCREEN_WIDTH / 2 - textSize.x / 2),  (720 / 2 - textSize.y/2)), Colors::White, 0, Vector2(textSize.x / 2, textSize.y / 2), sizeLerp);
	}
	this->healthBar->draw(false);
	this->minimap->draw(false);
	UserInterface::getSpriteBatch()->End();
}

UIPlaying::UIPlaying() : initMinimap(true)
{
	Game::getGraphics().loadTexture("UI/quest");
	this->quest = Game::getGraphics().getTexturePointer("UI/quest");
	this->respawnTimer = 0;
}

UIPlaying::~UIPlaying()
{
}

void UIPlaying::init()
{
	this->healthBar = std::make_unique<Slider>(Vector2(10, 10));
	this->minimap = std::make_unique<Minimap>(0.25f, 25.0f, Vector2(SCREEN_WIDTH - 10, SCREEN_HEIGHT - 10) - Minimap::size);
}

void UIPlaying::resetMinimapFog()
{
	this->minimap->resetFog();
}
