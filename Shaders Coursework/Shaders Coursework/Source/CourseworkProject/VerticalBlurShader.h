#pragma once
#include "../DXFramework/BaseShader.h"
class VerticalBlurShader :
	public BaseShader
{

public:
	struct screenHeightType
	{
		float screenHeight;
		XMFLOAT3 padding;
	};

	struct weightBufferType
	{
		float weight0;
		float weight1;
		float weight2;
		float padding;
	};


	VerticalBlurShader(ID3D11Device* device, HWND hwnd);
	~VerticalBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, float screenHeight, ID3D11ShaderResourceView* texture, XMFLOAT3 weightings);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);
private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* screenHeightBuffer;
	ID3D11Buffer* weightingsBuffer;
};

