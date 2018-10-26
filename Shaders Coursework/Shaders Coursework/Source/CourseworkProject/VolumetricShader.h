#pragma once
#include "../DXFramework/BaseShader.h"
#include "..\DXFramework\Light.h"
#include <windows.media.devices.h>
class VolumetricShader :
	public BaseShader
{
public:

	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		DirectX::XMFLOAT2 screenPos;
		float decay;
		float exposure;
		XMFLOAT3 lightDirection;
		float weight;
	};

	VolumetricShader(ID3D11Device* device, HWND hwnd);
	~VolumetricShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, const XMMATRIX & orthoView, const XMMATRIX & orthoMatrix, ID3D11ShaderResourceView* texture, Light* light);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);
	DirectX::XMVECTOR WorldToScreenPoint(XMVECTOR Point2D, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, const XMMATRIX &worldMatrix, int width, int height);

private:
	void initShader(WCHAR*, WCHAR* );

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11BlendState* BlendState;
	ID3D11Buffer* lightBuffer;
};

