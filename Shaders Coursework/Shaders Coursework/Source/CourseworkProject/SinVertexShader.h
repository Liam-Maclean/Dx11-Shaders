// Simple shader example.
#ifndef _SINVERTEXSHADER_H_
#define _SINVERTEXSHADER_H_

#include "../DXFramework/BaseShader.h"

using namespace std;
using namespace DirectX;

class SinVertexShader :
	public BaseShader
{
public:
	SinVertexShader(ID3D11Device* device, HWND hwnd);
	~SinVertexShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, float time, float speed, float height);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* timeBuffer;
};
#endif
