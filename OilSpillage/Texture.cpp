#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "STB/stb_image.h"
#include <DDSTextureLoader.h>

Texture::Texture()
{
	this->texture = nullptr;
	this->textureView = nullptr;

	this->width = 0;
	this->height = 0;

	this->transparent = false;

	this->cpuOnly = false;
	this->data = nullptr;
	this->bpp = 0;
}

Texture::~Texture()
{
}

bool Texture::Initialize(ID3D11Device * device, ID3D11DeviceContext* deviceContext, std::wstring filename, int mipLevels, bool cpuOnly)
{
	this->cpuOnly = cpuOnly;

	if (filename.find(L".tga", 0) != std::wstring::npos)
	{
		std::string filenameConvert = std::string(filename.begin(), filename.end());
		
		unsigned char* targaData = stbi_load(filenameConvert.c_str(), &this->width, &this->height, &this->bpp, STBI_rgb_alpha);
		if (!targaData)
		{
			return false;
		}

		if (cpuOnly)
		{

			long imageSize = this->width * static_cast<long>(this->height)* static_cast<long>(this->bpp);
			this->data = new unsigned char[imageSize];
			CopyMemory(this->data, targaData, imageSize);

			stbi_image_free(targaData);
			return true;
		}

		// Setup the description of the texture.
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Height = this->height;
		textureDesc.Width = this->width;
		textureDesc.MipLevels = 0;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		if (this->bpp == 1)
		{
			textureDesc.Format = DXGI_FORMAT_R8_UNORM;
		}
		else if (this->bpp == 2)
		{
			textureDesc.Format = DXGI_FORMAT_R8G8_UNORM;
		}

		// Create the empty texture.
		HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &texture);
		if (FAILED(hResult))
		{
			return false;
		}
		
		// Set the row pitch of the targa image data.
		unsigned int rowPitch = (width * 4) * sizeof(unsigned char);

		// Copy the targa image data into the texture.
		deviceContext->UpdateSubresource(texture, 0, NULL, targaData, rowPitch, 0);

		// Setup the shader resource view description.
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = textureDesc.Format;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;//Miplevels in parameter

		hResult = device->CreateShaderResourceView(texture, &srvDesc, &textureView);
		if (FAILED(hResult))
		{
			return false;
		}

		//deviceContext->GenerateMips(textureView);

		stbi_image_free(targaData);
	}
	else
	{
		size_t maxSize = 4096 * 4096 * 32;
		HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
			device,
			deviceContext,
			filename.c_str(),
			maxSize,
			D3D11_USAGE_DEFAULT,
			(unsigned int)(D3D11_BIND_SHADER_RESOURCE),
			(unsigned int)0,
			(unsigned int)0,
			false,
			(ID3D11Resource * *)& texture,
			&textureView,
			(DirectX::DDS_ALPHA_MODE*) nullptr
		);

		//
		//auto gen mipmaps
		//
		//HRESULT hr = DirectX::CreateDDSTextureFromFileEx(
		//	device,
		//	deviceContext,
		//	filename,
		//	maxSize,
		//	D3D11_USAGE_DEFAULT,
		//	(unsigned int)(D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET),
		//	(unsigned int)0,
		//	(unsigned int)D3D11_RESOURCE_MISC_GENERATE_MIPS,
		//	false,
		//	(ID3D11Resource * *)& texture,
		//	&textureView,
		//	(DirectX::DDS_ALPHA_MODE*) nullptr
		//);
	}
	return true;
}

bool Texture::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int width, int height)
{
	this->width = width;
	this->height = height;

	// Setup the description of the texture.
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Height = this->height;
	textureDesc.Width = this->width;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	// Create the empty texture.
	HRESULT hResult = device->CreateTexture2D(&textureDesc, NULL, &texture);
	if (FAILED(hResult))
	{
		return false;
	}

	// Setup the shader resource view description.
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;//Miplevels in parameter

	hResult = device->CreateShaderResourceView(texture, &srvDesc, &textureView);
	if (FAILED(hResult))
	{
		return false;
	}

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

	// Release the data.
	if (data)
	{
		delete data;
		data = 0;
	}

	return;
}

ID3D11Texture2D* Texture::getTexture2D()
{
	return texture;
}

ID3D11ShaderResourceView* Texture::getShaderResView()
{
	return textureView;
}

bool Texture::isTransparent()
{
	return this->transparent;
}

bool Texture::isCpuOnly()
{
	return this->cpuOnly;
}

unsigned char* Texture::getData()
{
	return this->data;
}

long Texture::getDataSize()
{
	return this->width * static_cast<long>(this->height) * static_cast<long>(this->bpp);
}

int Texture::getBytesPerPixel()
{
	return this->bpp;
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
