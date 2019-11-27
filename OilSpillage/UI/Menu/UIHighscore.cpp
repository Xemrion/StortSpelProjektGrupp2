#include "UIHighscore.h"
#include "../../MyClient.h"
#include "../../Input.h"
#include "../../States/MenuGameState.h"

void UIHighscore::updateUI(float deltaTime)
{
	this->highscore->update(deltaTime);

	if (this->loadingMutex.try_lock())
	{
		if (timer < 0.25f * 10)
		{
			this->timer += deltaTime;
		}

		for (int i = 0; i < 10; i++)
		{
			if (this->scoreText[i].get() && this->timer > 0.25f * i) this->scoreText[i]->update(deltaTime);
		}

		if (this->noConnection.get()) this->noConnection->update(deltaTime);

		if (Input::checkButton(Keys::CONFIRM, States::PRESSED) || Input::checkButton(Keys::CANCEL, States::PRESSED))
		{
			static_cast<MenuGameState*>(Game::getCurrentState())->setCurrentMenu(MenuGameState::MENU_MAIN);
		}

		this->loadingMutex.unlock();
	}
	else
	{
		this->loading->update(deltaTime);
	}
}

void UIHighscore::drawUI()
{
	UserInterface::getSpriteBatch()->Begin(SpriteSortMode_Deferred, UserInterface::getCommonStates()->NonPremultiplied());
	this->highscore->draw(false);

	if (this->loadingMutex.try_lock())
	{
		for (int i = 0; i < 10; i++)
		{
			if (this->scoreText[i].get()) this->scoreText[i]->draw(false);
		}
		this->loadingMutex.unlock();

		if (this->noConnection.get()) this->noConnection->draw(false);
		this->promptBar->draw(false);
	}
	else
	{
		this->loading->draw(false);
	}
	
	UserInterface::getSpriteBatch()->End();
}

UIHighscore::UIHighscore() : timer(0.0f)
{
}

UIHighscore::~UIHighscore()
{
	if (this->sendHighscore.joinable()) this->sendHighscore.join();
}

void UIHighscore::init(bool send, std::string name)
{
	this->highscore = std::make_unique<AnimatedText>("Highscore", Color(Colors::White), 1.0f, Animation::UP_DOWN_FADE_IN);
	this->highscore->setPosition(Vector2(SCREEN_WIDTH / 2 - this->highscore->getSize().x / 2, 10.0f));
	this->highscore->beginAnimation();
	this->loading = std::make_unique<AnimatedText>("Connecting to server...", Color(Colors::White), 0.5f, Animation::UP_DOWN_FADE_IN);
	this->loading->setPosition(Vector2(SCREEN_WIDTH / 2 - this->loading->getSize().x / 2, SCREEN_HEIGHT / 2 - this->loading->getSize().y / 2));
	this->loading->beginAnimation();

	Prompt prompts[] = {
		{ Keys::CONFIRM, "Confirm", Color(Colors::White) },
		{ Keys::CANCEL, "Cancel/Back", Color(Colors::White) }
	};

	this->promptBar = std::make_unique<ButtonPromptBar>(prompts, 2);
	this->promptBar->setPositon(Vector2(SCREEN_WIDTH / 2 - this->promptBar->getSize().x / 2, SCREEN_HEIGHT - this->promptBar->getSize().y - 8.0f));

	this->timer = 0.0f;

	if (this->sendHighscore.joinable()) this->sendHighscore.join();
	this->sendHighscore = std::thread(&UIHighscore::loadHighscore, this, send, name);
}

void UIHighscore::loadHighscore(UIHighscore* ui, bool send, std::string name)
{
	while (!ui->loadingMutex.try_lock()) {}

	std::vector<PlayerStat> stats;
	Color color;
	MyClient client;

	if (PNet::Network::Initialize() && client.Connect(PNet::IPEndpoint("192.168.43.233", 6113)))
	{
		if (send)
		{
			client.SendPlayerStats(PlayerStat(name, Game::getGameInfo().highScore));
		}
		else
		{
			client.SendPlayerStats(PlayerStat("Unknown", 0));
		}
		
		while (client.IsConnected())
		{
			client.Frame();
		}

		PNet::Network::Shutdown();

		stats = client.getStats();
		ui->noConnection.reset();
	}
	else
	{
		std::ifstream localHighscore("highscore.txt");
		std::ofstream localHighscoreOut;

		if (!localHighscore.is_open())
		{
			try
			{
				localHighscoreOut.open("highscore.txt");
				localHighscoreOut << "0";
				localHighscoreOut.close();

				localHighscore.open("highscore.txt");
			}
			catch (...) {}
		}

		if (localHighscore.is_open())
		{
			int players = 0;
			std::string name;
			std::string score;
			std::getline(localHighscore, name);

			try
			{
				players = std::stoi(name);
			}
			catch (...) {}

			for (int i = 0; i < min(players, 10); i++)
			{
				std::getline(localHighscore, name);
				std::getline(localHighscore, score);
				stats.push_back(PlayerStat(name, std::stoi(score)));
			}

			localHighscore.close();

			if (send)
			{
				stats.push_back(PlayerStat(name, Game::getGameInfo().highScore));
				std::sort(stats.begin(), stats.end(), [](const PlayerStat& a, const PlayerStat& b) { return a.score > b.score; });
				if (stats.size() > 10) stats.resize(10);

				localHighscoreOut.open("highscore.txt", std::ios::trunc);
				localHighscoreOut << stats.size() << "\n";

				for (int i = 0; i < stats.size(); i++)
				{
					localHighscoreOut << stats[i].name << "\n";
					localHighscoreOut << stats[i].score << "\n";
				}

				localHighscoreOut.close();
			}
			
		}

		ui->noConnection = std::make_unique<AnimatedText>("Server is offline :(", Color(Colors::Red), 0.25f, Animation::SHAKING_FADE_IN_STOP);
		ui->noConnection->setPosition(Vector2(SCREEN_WIDTH - ui->noConnection->getSize().x - 5, SCREEN_HEIGHT - ui->noConnection->getSize().y - 5));
		ui->noConnection->beginAnimation();
	}

	for (int i = 0; i < 10; i++)
	{
		if (i < stats.size())
		{
			switch (i)
			{
			case 0:
				color = Color(255 / 255.0f, 215 / 255.0f, 0.0f / 255.0f); //Gold
				break;
			case 1:
				color = Color(192 / 255.0f, 192 / 255.0f, 192 / 255.0f); //Silver
				break;
			case 2:
				color = Color(205 / 255.0f, 127 / 255.0f, 50 / 255.0f); //Bronze
				break;
			default:
				color = Color(Colors::White);
				break;
			}

			ui->scoreText[i] = std::make_unique<AnimatedText>(stats[i].name + " -- " + std::to_string(stats[i].score) + " Points", color, 0.5f, Animation::SHAKING_FADE_IN_STOP);
			ui->scoreText[i]->setPosition(Vector2(SCREEN_WIDTH / 2 - ui->scoreText[i]->getSize().x / 2, 150 + 50 * i));
			ui->scoreText[i]->beginAnimation();
		}
		else
		{
			ui->scoreText[i].reset();
		}
	}

	ui->loadingMutex.unlock();
}
