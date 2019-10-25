#include "Minimap.h"
#include "UserInterface.h"
#include "../game.h"
#include "../States/PlayingGameState.h"
#include <cassert>

Vector2 Minimap::size = Vector2(96 * 2, 96 * 2);

Minimap::Minimap(float zoom, float fogClearRadius, Vector2 position)
	: Element(position, 0.0f), fogClearRadius(max(fogClearRadius, 10.0f)), zoom(std::clamp(zoom, 0.0f, 1.0f)),
							   textureMap(nullptr), textureFog(nullptr), resourceFog(nullptr)
{
	Game::getGraphics().loadTexture("UI/mapOutline");
	Game::getGraphics().loadTexture("UI/mapPlayerMarker");
	Game::getGraphics().loadTexture("UI/mapFog", false, true);
	this->textureOutline = Game::getGraphics().getTexturePointer("UI/mapOutline");
	this->texturePlayerMarker = Game::getGraphics().getTexturePointer("UI/mapPlayerMarker");
	this->textureFogTemp = Game::getGraphics().getTexturePointer("UI/mapFog");
	assert(textureOutline && "Texture failed to load!");
	assert(texturePlayerMarker && "Texture failed to load!");
	assert(textureFogTemp && "Texture failed to load!");

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

	SpriteBatch* sb = UserInterface::getSpriteBatch();
	sb->Draw(this->textureMap->getShaderResView(), mapRect, &zoomedRect);
	sb->Draw(this->resourceFog, mapRect, &zoomedRect);
	sb->Draw(this->texturePlayerMarker->getShaderResView(), markerRect, nullptr, Colors::White, playerRot, this->texturePlayerMarker->getCenter());
	sb->Draw(this->textureOutline->getShaderResView(), this->position - Vector2(5, 5));
}

void Minimap::update(float deltaTime)
{
	Vector3 playerMapPos = Vector3::Transform(static_cast<PlayingGameState*>(Game::getCurrentState())->getPlayer()->getVehicle()->getPosition() * Vector3(1, 0, 1), this->mapMatrix);
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
			if ((playerMapPos - Vector3(static_cast<float>(x), 0, static_cast<float>(y))).Length() < this->fogClearRadius)
			{
				this->pixels[(y * this->textureFogTemp->getWidth() + x) * 4 + 3] = 0;
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
}

void Minimap::resetFog()
{
	CopyMemory(this->pixels, this->textureFogTemp->getData(), this->textureFogTemp->getDataSize());
}
