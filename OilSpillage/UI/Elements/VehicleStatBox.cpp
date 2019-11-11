#include "VehicleStatBox.h"
#include "../UserInterface.h"
#include "../../game.h"

Vector2 VehicleStatBox::size = Vector2(256, 6 * 64);

VehicleStatBox::VehicleStatBox(Vector2 position) : Element(position), amounts{ 0 }
{
	Game::getGraphics().loadTexture("UI/sliderBG");
	Game::getGraphics().loadTexture("UI/statFG");
	this->textureBG = Game::getGraphics().getTexturePointer("UI/sliderBG");
	this->textureFG = Game::getGraphics().getTexturePointer("UI/statFG");
	assert(textureBG && "Texture failed to load!");
	assert(textureFG && "Texture failed to load!");

	for (int i = 0; i < 6; i++)
	{
		this->amounts[i] = (rand() % 11) / 10.0f;
	}
}

VehicleStatBox::~VehicleStatBox()
{
}

void VehicleStatBox::draw(bool selected)
{
	SpriteBatch* sb = UserInterface::getSpriteBatch();
	SpriteFont* font = UserInterface::getFontArial();
	RECT bgDest;
	RECT amountDest;

	for (int i = 0; i < 6; i++)
	{
		bgDest = SimpleMath::Rectangle(static_cast<long>(this->position.x + 8), static_cast<long>(this->position.y + 8 + i * 54 + 22), static_cast<long>(this->textureFG->getWidth() - 16), static_cast<long>(this->textureBG->getHeight()));
		amountDest = SimpleMath::Rectangle(static_cast<long>(this->position.x + 8), static_cast<long>(this->position.y + 8 + i * 54 + 22), static_cast<long>((this->textureFG->getWidth() - 16) * this->amounts[i]), static_cast<long>(this->textureBG->getHeight()));

		sb->Draw(this->textureBG->getShaderResView(), bgDest, Colors::White);
		sb->Draw(this->textureBG->getShaderResView(), amountDest, Colors::LimeGreen);
		sb->Draw(this->textureFG->getShaderResView(), this->position + Vector2(0, i * 54 + 22));
	}

	font->DrawString(sb, "Health",			this->position + Vector2(8, 0 * 54), Colors::White, 0.0f, Vector2::Zero, 0.2f);
	font->DrawString(sb, "Durability",		this->position + Vector2(8, 1 * 54), Colors::White, 0.0f, Vector2::Zero, 0.2f);
	font->DrawString(sb, "Armour",			this->position + Vector2(8, 2 * 54), Colors::White, 0.0f, Vector2::Zero, 0.2f);
	font->DrawString(sb, "Acceleration",	this->position + Vector2(8, 3 * 54), Colors::White, 0.0f, Vector2::Zero, 0.2f);
	font->DrawString(sb, "Max Speed",		this->position + Vector2(8, 4 * 54), Colors::White, 0.0f, Vector2::Zero, 0.2f);
	font->DrawString(sb, "Handling",		this->position + Vector2(8, 5 * 54), Colors::White, 0.0f, Vector2::Zero, 0.2f);
}

void VehicleStatBox::update(const Stats& stats)
{
	this->amounts[0] = stats.maxHealth / (VehicleStats::defaultStats.maxHealth * 2.0f);
	this->amounts[1] = stats.durability / (VehicleStats::defaultStats.durability * 2.0f);
	this->amounts[2] = stats.armour / (VehicleStats::defaultStats.armour * 2.0f);
	this->amounts[3] = stats.accelerationRate / (VehicleStats::defaultStats.accelerationRate * 2.0f);
	this->amounts[4] = stats.maxSpeed / (VehicleStats::defaultStats.maxSpeed * 2.0f);
	this->amounts[5] = stats.handlingRate / (VehicleStats::defaultStats.handlingRate * 2.0f);
}