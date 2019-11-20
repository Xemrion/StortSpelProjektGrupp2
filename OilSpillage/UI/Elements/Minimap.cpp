#include "Minimap.h"
#include "../UserInterface.h"
#include "../../game.h"
#include "../../States/PlayingGameState.h"
#include <cassert>

Vector2 Minimap::size = Vector2(96 * 2, 96 * 2);

Minimap::Minimap(float zoom, float fogClearRadius, Vector2 position)
	: Element(position, 0.0f), fogClearRadius(max(fogClearRadius, 10.0f)), zoom(std::clamp(zoom, 0.0f, 1.0f)),
							   textureMap(nullptr), textureFog(nullptr), resourceFog(nullptr), compassRot(0.0f)
{
	Game::getGraphics().loadTexture("UI/mapOutline");
	Game::getGraphics().loadTexture("UI/mapPlayerMarker");
	Game::getGraphics().loadTexture("UI/mapObjective");
	Game::getGraphics().loadTexture("UI/mapEnemy");
	Game::getGraphics().loadTexture("UI/mapCompass");
	Game::getGraphics().loadTexture("UI/mapFog", false, true);

	this->textureOutline = Game::getGraphics().getTexturePointer("UI/mapOutline");
	this->texturePlayerMarker = Game::getGraphics().getTexturePointer("UI/mapPlayerMarker");
	this->textureObjectiveMarker = Game::getGraphics().getTexturePointer("UI/mapObjective");
	this->textureEnemyMarker = Game::getGraphics().getTexturePointer("UI/mapEnemy");
	this->textureCompass = Game::getGraphics().getTexturePointer("UI/mapCompass");
	this->textureFogTemp = Game::getGraphics().getTexturePointer("UI/mapFog");

	assert(textureOutline && "Texture failed to load!");
	assert(texturePlayerMarker && "Texture failed to load!");
	assert(textureFogTemp && "Texture failed to load!");
	assert(textureEnemyMarker && "Texture failed to load!");
	assert(textureCompass && "Texture failed to load!");

	ID3D11Device* device = Game::getGraphics().getDevice();
	ID3D11DeviceContext* deviceContext = Game::getGraphics().getDeviceContext();

	// Setup the description of the texture.
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Height = this->textureFogTemp->getWidth();
	textureDesc.Width = this->textureFogTemp->getHeight();
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DYNAMIC;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	textureDesc.MiscFlags = 0;

	// Create an empty texture.
	HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &this->textureFog);
	if (FAILED(hResult))
	{
		assert(this->textureFog && "Texture failed to create!");
	}
	
	// Setup the shader resource view description.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	hResult = device->CreateShaderResourceView(this->textureFog, &srvDesc, &this->resourceFog);
	if (FAILED(hResult))
	{
		assert(this->textureFog && "Texture failed to create!");
	}

	//Copy the texture so we can change the alpha later
	this->pixels = new unsigned char[this->textureFogTemp->getDataSize()];
	CopyMemory(this->pixels, this->textureFogTemp->getData(), this->textureFogTemp->getDataSize());
}

Minimap::~Minimap()
{
	delete this->pixels;
	this->textureFog->Release();
	this->resourceFog->Release();
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
	float playerRot = state->getPlayer()->getVehicle()->getRotation().y;
	Vector3 playerPos(state->getPlayer()->getVehicle()->getPosition() * Vector3(1, 0, 1));
	Vector3 minimapSize(this->textureMap->getWidth(), 1, this->textureMap->getHeight());
	Vector3 zoomedMinimapSize(minimapSize * zoom);
	Vector3 zoomedMinimapScale(Vector3(Minimap::size.x, 0, Minimap::size.y) / zoomedMinimapSize);

	Vector3 playerMapPos = Vector3::Transform(playerPos, this->mapMatrix);
	playerMapPos.Clamp(Vector3(), Vector3(this->textureMap->getWidth(), 0, this->textureMap->getHeight()));
	playerMapPos.z = this->textureMap->getHeight() - playerMapPos.z;
	Vector3 mapCamPos = playerMapPos - zoomedMinimapSize * 0.5f;
	mapCamPos.Clamp(Vector3(), minimapSize - zoomedMinimapSize);
	Vector3 playerZoomedPos = (playerMapPos - mapCamPos) * zoomedMinimapScale;
	playerZoomedPos.Clamp(Vector3(), Vector3(Minimap::size.x, 0, Minimap::size.y));

	
	RECT zoomedRect = SimpleMath::Rectangle(
		static_cast<long>(mapCamPos.x), static_cast<long>(mapCamPos.z),
		static_cast<long>(zoomedMinimapSize.x), static_cast<long>(zoomedMinimapSize.z)
	);
	/*RECT zoomedFogRect = SimpleMath::Rectangle(
		static_cast<long>(mapCamPos.x / 3), static_cast<long>(mapCamPos.z / 3),
		static_cast<long>(zoomedMinimapSize.x / 3), static_cast<long>(zoomedMinimapSize.z / 3)
	);*/
	RECT mapRect = SimpleMath::Rectangle(
		static_cast<long>(this->position.x), static_cast<long>(this->position.y),
		static_cast<long>(Minimap::size.x), static_cast<long>(Minimap::size.y)
	);
	RECT markerRect = SimpleMath::Rectangle(
		static_cast<long>(this->position.x + playerZoomedPos.x), static_cast<long>(this->position.y + playerZoomedPos.z),
		static_cast<long>(this->texturePlayerMarker->getWidth() * zoomedMinimapScale.x), static_cast<long>(this->texturePlayerMarker->getHeight() * zoomedMinimapScale.z)
	);
	RECT compassRect = SimpleMath::Rectangle(
		static_cast<long>(this->position.x + playerZoomedPos.x), static_cast<long>(this->position.y + playerZoomedPos.z),
		static_cast<long>(this->textureCompass->getWidth() * zoomedMinimapScale.x), static_cast<long>(this->textureCompass->getHeight() * zoomedMinimapScale.z)
	);
	

	SpriteBatch* sb = UserInterface::getSpriteBatch();
	sb->Draw(this->textureMap->getShaderResView(), mapRect, &zoomedRect);
	//<test
	Vector3 targetPos;
	Vector3 targetMapPos;
	Vector3 targetZoomedPos;
	RECT targetRect;



	for (int i = 0; i < static_cast<PlayingGameState*>(Game::getCurrentState())->actorManager->getGroups().size(); i++)
	{
		targetPos = static_cast<PlayingGameState*>(Game::getCurrentState())->actorManager->getGroups()[i].getAveragePos();
		if ((targetPos - playerPos).Length() < 150.0f)
		{
			targetMapPos = Vector3::Transform(targetPos, this->mapMatrix);
			targetMapPos.Clamp(Vector3(), Vector3(this->textureMap->getWidth(), 0, this->textureMap->getHeight()));
			targetMapPos.z = this->textureMap->getHeight() - targetMapPos.z;
			targetZoomedPos = (targetMapPos - mapCamPos) * zoomedMinimapScale;
			targetZoomedPos.Clamp(Vector3(), Vector3(Minimap::size.x, 0, Minimap::size.y));
			targetRect = SimpleMath::Rectangle(
				static_cast<long>(this->position.x + targetZoomedPos.x), static_cast<long>(this->position.y + targetZoomedPos.z),
				static_cast<long>(this->textureEnemyMarker->getWidth() * zoomedMinimapScale.x), static_cast<long>(this->textureEnemyMarker->getHeight() * zoomedMinimapScale.z)
			);

			sb->Draw(this->textureEnemyMarker->getShaderResView(), targetRect, nullptr, Colors::White, 0, this->textureEnemyMarker->getCenter());
		}
	}

	if (state->getObjHandler().getObjective(0) != nullptr)
	{
		for (int i = 0; i < state->getObjHandler().getObjective(0)->getNrOfMax(); i++)
		{
			if (state->getObjHandler().getObjective(0)->getType() != TypeOfMission::KillingSpree)
			{
				if (state->getObjHandler().getObjective(0)->getTarget(i) != nullptr)
				{
					targetPos = state->getObjHandler().getObjective(0)->getTarget(i)->getPosition();
					if ((targetPos - playerPos).Length() < 300.0f)
					{
						targetMapPos = Vector3::Transform(targetPos, this->mapMatrix);
						targetMapPos.Clamp(Vector3(), Vector3(this->textureMap->getWidth(), 0, this->textureMap->getHeight()));
						targetMapPos.z = this->textureMap->getHeight() - targetMapPos.z;
						targetZoomedPos = (targetMapPos - mapCamPos) * zoomedMinimapScale;
						targetZoomedPos.Clamp(Vector3(), Vector3(Minimap::size.x, 0, Minimap::size.y));
						targetRect = SimpleMath::Rectangle(
							static_cast<long>(this->position.x + targetZoomedPos.x), static_cast<long>(this->position.y + targetZoomedPos.z),
							static_cast<long>(this->textureObjectiveMarker->getWidth() * zoomedMinimapScale.x), static_cast<long>(this->textureObjectiveMarker->getHeight() * zoomedMinimapScale.z)
						);

						sb->Draw(this->textureObjectiveMarker->getShaderResView(), targetRect, nullptr, Colors::White, 0, this->textureObjectiveMarker->getCenter());
					}
				}
			}
		}
	}
	
	sb->Draw(this->textureOutline->getShaderResView(), this->position - Vector2(5, 5));
	sb->Draw(this->resourceFog, mapRect, &zoomedRect);
	sb->Draw(this->texturePlayerMarker->getShaderResView(), markerRect, nullptr, Colors::White, playerRot, this->texturePlayerMarker->getCenter());

	if (state->getObjHandler().getObjective(0) != nullptr && state->getObjHandler().getObjective(0)->getType() != TypeOfMission::KillingSpree)
	{
		sb->Draw(this->textureCompass->getShaderResView(), compassRect, nullptr, Colors::White, this->compassRot, Vector2(this->textureCompass->getWidth() * 0.5f, this->textureCompass->getHeight() * 0.75f));
	}
}

void Minimap::update(float deltaTime)
{
	PlayingGameState* state = static_cast<PlayingGameState*>(Game::getCurrentState());
	Vector3 playerPos(state->getPlayer()->getVehicle()->getPosition() * Vector3(1, 0, 1));



	Vector3 playerMapPos = Vector3::Transform(playerPos, this->mapMatrix);
	//playerMapPos /= 3;
	playerMapPos.Clamp(Vector3(), Vector3(this->textureFogTemp->getWidth(), 0, this->textureFogTemp->getHeight()));
	playerMapPos.z = this->textureFogTemp->getHeight() - playerMapPos.z;

	Vector2 topLeft = Vector2(max(playerMapPos.x - this->fogClearRadius, 0),
							  max(playerMapPos.z - this->fogClearRadius, 0));
	Vector2 bottomRight = Vector2(min(playerMapPos.x + this->fogClearRadius, this->textureFogTemp->getWidth()),
							      min(playerMapPos.z + this->fogClearRadius, this->textureFogTemp->getHeight()));

	for (int x = static_cast<int>(topLeft.x); x < static_cast<int>(std::roundf(bottomRight.x)); x++)
	{
		for (int y = static_cast<int>(topLeft.y); y < static_cast<int>(std::roundf(bottomRight.y)); y++)
		{
			float shade = 255;
			if ((playerMapPos - Vector3(static_cast<float>(x), 0, static_cast<float>(y))).Length() < this->fogClearRadius)
			{
				shade = 150;
			}
			if ((playerMapPos - Vector3(static_cast<float>(x), 0, static_cast<float>(y))).Length() < this->fogClearRadius - 5.0f)
			{
				shade = 100;
			}
			if ((playerMapPos - Vector3(static_cast<float>(x), 0, static_cast<float>(y))).Length() < this->fogClearRadius - 10.0f)
			{
				shade = 50;
			}
			if ((playerMapPos - Vector3(static_cast<float>(x), 0, static_cast<float>(y))).Length() < this->fogClearRadius-15.0f)
			{
				shade = 0;
			}
			if (this->pixels[(y * this->textureFogTemp->getWidth() + x) * 4 + 3] > shade) {
				this->pixels[(y * this->textureFogTemp->getWidth() + x) * 4 + 3] = shade;
			}
		}
	}

	ID3D11DeviceContext* deviceContext = Game::getGraphics().getDeviceContext();
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	HRESULT hResult = deviceContext->Map(this->textureFog, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(hResult))
	{
		assert("Failed to map texture!");
	}

	CopyMemory(mappedResource.pData, this->pixels, this->textureFogTemp->getDataSize());
	deviceContext->Unmap(this->textureFog, 0);

	Vector3 closestPos = state->getObjHandler().getObjective(0)->getClosestToPlayer();
	this->compassRot = XMVector3AngleBetweenVectors(closestPos - playerPos, Vector3::UnitX).m128_f32[0] + XM_PIDIV2;

	if (playerPos.z < closestPos.z)
	{
		this->compassRot = XM_PI - this->compassRot;
	}
}

void Minimap::resetFog()
{
	CopyMemory(this->pixels, this->textureFogTemp->getData(), this->textureFogTemp->getDataSize());
}

bool Minimap::hasExplored(Vector3 worldPosition) const
{
	Vector3 posOnMap = Vector3::Transform(worldPosition, this->mapMatrix);

	if (posOnMap.x < 0 || posOnMap.x > this->textureFogTemp->getWidth() || posOnMap.z < 0 || posOnMap.z > this->textureFogTemp->getHeight())
	{
		return false;
	}

	posOnMap.z = this->textureFogTemp->getHeight() - posOnMap.z;
	return this->pixels[(static_cast<int>(posOnMap.z) * this->textureFogTemp->getWidth() + static_cast<int>(posOnMap.x)) * 4 + 3] != 255;
}
