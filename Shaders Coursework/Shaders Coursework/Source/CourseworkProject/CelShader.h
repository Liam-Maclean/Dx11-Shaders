// Colour shader.h
// Simple shader example.
#ifndef _CELSHADER_H_
#define _CELSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Light.h"
using namespace std;
using namespace DirectX;


class CelShader : public BaseShader
{
	struct lightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};


public:

	CelShader(ID3D11Device* device, HWND hwnd);
	~CelShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Light* light);
	void render(ID3D11DeviceContext* deviceContext, int indexCount);


private:

	lightBufferType* lightPtr;
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11RasterizerState* rasterBuffer;
};

#endif