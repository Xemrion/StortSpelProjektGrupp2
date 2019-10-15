#ifndef Texture_H
#define Texture_H
#include <d3d11.h>
#include <stdio.h>
#include <fstream>
#include <SimpleMath.h>

class Texture
{
private:
	struct TargaHeader
	{
		unsigned char data1[12];
		unsigned short width;
		unsigned short height;
		unsigned char bpp;
		unsigned char data2;
	};

	bool LoadTarga(const char*, int&, int&);

	unsigned char* targaData;
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* textureView;

	//TO STORE WIDTH & HEIGHT
	int width;
	int height;

	bool transparent;
public:
	Texture();
	Texture(const Texture&);
	~Texture();

	bool Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const char* filename, int mipLevels);
	void Shutdown();

	ID3D11ShaderResourceView* getShaderResView();

	bool isTransparent();

	unsigned char* getTextureCharArray();
	unsigned short getWidth();
	unsigned short getHeight();
	DirectX::SimpleMath::Vector2 getSize();
	DirectX::SimpleMath::Vector2 getCenter();
};

#endif