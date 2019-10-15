#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
Texture::Texture()
{
	this->texture = nullptr;
	this->textureView = nullptr;

	this->width = 0;
	this->height = 0;

	this->transparent = false;
}

Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device * device, ID3D11DeviceContext* deviceContext, const char* filename, int mipLevels)
{
	

	bool result;
	D3D11_TEXTURE2D_DESC textureDesc;
	HRESULT hResult;
	unsigned int rowPitch;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	int bpp = 0;
	unsigned char* targaData = stbi_load(filename, &this->width, &this->height, &bpp, STBI_rgb_alpha);

	if (!targaData)
	{
		return false;
	}

	// Setup the description of the texture.
	textureDesc.Height = this->height;
	textureDesc.Width = this->width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	// Create the empty texture.
	hResult = device->CreateTexture2D(&textureDesc, NULL, &texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// Set the row pitch of the targa image data.
	rowPitch = (width * 4) * sizeof(unsigned char);

	// Copy the targa image data into the texture.
	deviceContext->UpdateSubresource(texture, 0, NULL, targaData, rowPitch, 0);

	// Setup the shader resource view description.
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = mipLevels;//Miplevels in parameter

	hResult = device->CreateShaderResourceView(texture, &srvDesc, &textureView);
	if (FAILED(hResult))
	{
		return false;
	}

	deviceContext->GenerateMips(textureView);

	stbi_image_free(targaData);
	return true;
}

void Texture::Shutdown()
{
	// Release the texture view resource.
	if (textureView)
	{
		textureView->Release();
		textureView = 0;
	}

	// Release the texture.
	if (texture)
	{
		texture->Release();
		texture = 0;
	}

	return;
}

ID3D11ShaderResourceView* Texture::getShaderResView()
{
	return textureView;
}

bool Texture::isTransparent()
{
	return this->transparent;
}

unsigned short Texture::getWidth()
{
	return this->width;
}

unsigned short Texture::getHeight()
{
	return this->height;
}

DirectX::SimpleMath::Vector2 Texture::getSize()
{
	return DirectX::SimpleMath::Vector2(static_cast<float>(this->width), static_cast<float>(this->height));
}

DirectX::SimpleMath::Vector2 Texture::getCenter()
{
	return DirectX::SimpleMath::Vector2(static_cast<float>(this->width), static_cast<float>(this->height)) / 2;
}
