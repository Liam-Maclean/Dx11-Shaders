// Colour shader.h
// Simple shader example.
#ifndef _SILHOUETTESHADER_H_
#define _SILHOUETTESHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Light.h"
using namespace std;
using namespace DirectX;


class SilhouetteShader : public BaseShader
{
	struct colorBufferType
	{
		XMFLOAT4 color;
	};


public:

	SilhouetteShader(ID3D11Device* device, HWND hwnd);
	~SilhouetteShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, XMFLOAT4 color);
	void render(ID3D11DeviceContext* deviceContext, int indexCount);


private:

	colorBufferType* colorPtr;
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* colorBuffer;
	ID3D11RasterizerState* rasterBuffer;
};

#endif