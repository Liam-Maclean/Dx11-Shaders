// Colour shader.h
// Simple shader example.
#ifndef _COLOURSHADER_H_
#define _COLOURSHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;


class ColourShader : public BaseShader
{
	struct colourBufferType
	{
		XMFLOAT4 colour;
	};
	

public:

	ColourShader(ID3D11Device* device, HWND hwnd);
	~ColourShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, XMFLOAT4 colour);

private:
	colourBufferType* colourPtr;

	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* colourBuffer;
};

#endif