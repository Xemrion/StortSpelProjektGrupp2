#include "Minimap.h"
#include "UserInterface.h"
#include "../game.h"
#include <cassert>
#include "../States/PlayingGameState.h"

Vector2 Minimap::size = Vector2(96 * 3, 96 * 3);

Minimap::Minimap(float zoom, Vector2 position) : Element(position, 0.0f), zoom(std::clamp(zoom, 0.0f, 1.0f)), textureMap(nullptr)
{
	//Game::getGraphics().loadTexture("UI/mapOutline");
	Game::getGraphics().loadTexture("UI/mapPlayerMarker");
	//this->textureOutline = Game::getGraphics().getTexturePointer("UI/mapOutline");
	this->texturePlayerMarker = Game::getGraphics().getTexturePointer("UI/mapPlayerMarker");
	//assert(textureOutline && "Texture failed to load!");
	assert(texturePlayerMarker && "Texture failed to load!");
}

Minimap::~Minimap()
{
}

void Minimap::init()
{
	PlayingGameState* state = static_cast<PlayingGameState*>(Game::getCurrentState());
	std::string minimap = state->getMinimap();

	Game::getGraphics().loadTexture(minimap);
	this->textureMap = Game::getGraphics().getTexturePointer(minimap.c_str());
	assert(textureMap && "Texture failed to load!");

	Vector3 mapSize((state->getBottomRight() - state->getTopLeft() + Vector3(0, 1, 0)) * Vector3(1, 1, -1));
	Vector3 mapScale(Vector3(this->textureMap->getWidth(), 1, this->textureMap->getHeight()) / mapSize);

	this->mapMatrix = Matrix::CreateTranslation(-Vector3(state->getTopLeft().x, 0, state->getBottomRight().z));
	this->mapMatrix *= Matrix::CreateScale(mapScale);
}

void Minimap::draw(bool selected)
{
	PlayingGameState* state = static_cast<PlayingGameState*>(Game::getCurrentState());
	//float playerRot = state->getPlayer()->getVehicle()->getRotation().y;
	Vector3 playerPos(state->getPlayer()->getVehicle()->getPosition() * Vector3(1, 0, 1));
	Vector3 minimapSize(this->textureMap->getWidth(), 0, this->textureMap->getHeight());
	Vector3 zoomedMinimapSize(minimapSize * zoom);

	Vector3 playerMapPos = Vector3::Transform(playerPos, this->mapMatrix);
	playerMapPos.Clamp(Vector3(), Vector3(this->textureMap->getWidth(), 0, this->textureMap->getHeight()));
	playerMapPos.z = this->textureMap->getHeight() - playerMapPos.z;
	Vector3 mapCamPos = playerMapPos - zoomedMinimapSize * 0.5f;
	mapCamPos.Clamp(Vector3(), minimapSize - zoomedMinimapSize);
	Vector3 playerZoomedPos = playerMapPos - mapCamPos;
	playerZoomedPos.Clamp(Vector3(), zoomedMinimapSize);
	playerZoomedPos *= 1 / zoom;

	RECT zoomedRect = SimpleMath::Rectangle(mapCamPos.x, mapCamPos.z, zoomedMinimapSize.x, zoomedMinimapSize.z);
	RECT mapRect = SimpleMath::Rectangle(this->position.x, this->position.y, minimapSize.x, minimapSize.z);

	RECT markerRect = SimpleMath::Rectangle(
		this->position.x + playerZoomedPos.x + (1 / zoom), this->position.y + playerZoomedPos.z + (1 / zoom),
		this->texturePlayerMarker->getWidth() * (1 / zoom), this->texturePlayerMarker->getHeight() * (1 / zoom)
	);

	SpriteBatch* sb = UserInterface::getSpriteBatch();
	sb->Draw(this->textureMap->getShaderResView(), mapRect, &zoomedRect);
	sb->Draw(this->texturePlayerMarker->getShaderResView(), markerRect, nullptr, Colors::White, 0, this->texturePlayerMarker->getCenter());
}
