// Light shader.h
// Basic single light shader setup
#ifndef _SPOTLIGHTSHADER_H_
#define _SPOTLIGHTSHADER_H_

#include "../DXFramework/BaseShader.h"
#include "../DXFramework/Light.h"
#include "../DXFramework/Camera.h"
#include <vector>
using namespace std;
using namespace DirectX;


class SpotLightShader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT4 ambientColor;
		XMFLOAT3 lightTarget;
		float lightRange;
		XMFLOAT3 position;
		float coneRadiusAngle;
		XMFLOAT3 padding;
		float lightAttenuation;
	};


public:

	SpotLightShader(ID3D11Device* device, HWND hwnd);
	~SpotLightShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Light* light, float attenuation, float lightRange, XMFLOAT3 lightTarget);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);

private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* cameraBuffer;
	ID3D11Buffer* cameraVSBuffer;
};

#endif