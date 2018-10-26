// Light shader.h
// Geometry shader example.
#ifndef _GEOMETRYSHADER_H_
#define _GEOMETRYSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/DXF.h"
using namespace std;
using namespace DirectX;


class GeometryShader : public BaseShader
{

public:

	struct lightBufferType
	{
		XMMATRIX normalWorld;
		XMFLOAT4 lightColor;
		XMFLOAT3 lightPosition;
		float padding1;
		XMFLOAT3 lightTarget;
		float padding2;
		
	};

	GeometryShader(ID3D11Device* device, HWND hwnd);
	~GeometryShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Light* m_light, XMFLOAT3 lightTarget, const XMMATRIX &normalWorld);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR* vsFilename, WCHAR* psFilename);
	void initShader(WCHAR* vsFilename, WCHAR* gsFilename, WCHAR* psFilename);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* lightBuffer;
	ID3D11SamplerState* sampleState;
	lightBufferType* lightBufferPtr;

};

#endif