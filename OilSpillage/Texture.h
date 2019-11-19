#ifndef Texture_H
#define Texture_H
#include <d3d11.h>
#include <stdio.h>
#include <fstream>
#include <SimpleMath.h>

class Texture
{
private:
	friend class Graphics;
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* textureView;

	int width;
	int height;

	bool transparent;

	bool cpuOnly;
	unsigned char* data;
	int bpp;

public:
	Texture();
	Texture(const Texture&) = delete;
	~Texture();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename, int mipLevels, bool cpuOnly);
	void Shutdown();

	ID3D11Texture2D* getTexture2D();
	ID3D11ShaderResourceView* getShaderResView();

	bool isTransparent();

	bool isCpuOnly();
	unsigned char* getData();
	long getDataSize();
	int getBytesPerPixel();

	unsigned short getWidth();
	unsigned short getHeight();
	DirectX::SimpleMath::Vector2 getSize();
	DirectX::SimpleMath::Vector2 getCenter();
};

#endif