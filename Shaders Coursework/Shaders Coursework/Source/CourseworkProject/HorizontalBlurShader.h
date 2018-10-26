#pragma once
#include "../DXFramework/BaseShader.h"
class HorizontalBlurShader :
	public BaseShader
{
public:
	struct ScreenWidthType
	{
		float ScreenWidth;
		XMFLOAT3 padding;
	};

	struct WeightBufferType
	{
		float weight0;
		float weight1;
		float weight2;
		float padding;
	};


	HorizontalBlurShader(ID3D11Device* device, HWND hwnd);
	~HorizontalBlurShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, float screenWidth, ID3D11ShaderResourceView* texture, XMFLOAT3 weightings);
	void render(ID3D11DeviceContext* deviceContext, int vertexCount);
private:
	void initShader(WCHAR*, WCHAR*);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* ScreenWidthBuffer;
	ID3D11Buffer* weightBuffer;
};

