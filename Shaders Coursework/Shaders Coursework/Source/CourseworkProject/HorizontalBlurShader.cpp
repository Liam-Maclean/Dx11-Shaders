#include "HorizontalBlurShader.h"

HorizontalBlurShader::HorizontalBlurShader(ID3D11Device * device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"horizontalblur_vs.cso", L"horizontalblur_ps.cso");
}

HorizontalBlurShader::~HorizontalBlurShader()
{
}

void HorizontalBlurShader::setShaderParameters(ID3D11DeviceContext * deviceContext, const XMMATRIX & world, const XMMATRIX & view, const XMMATRIX & projection, float screenWidth, ID3D11ShaderResourceView* texture, XMFLOAT3 weightings)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource, mappedResource2;
	MatrixBufferType* dataPtr;
	WeightBufferType* weightPtr;
	ScreenWidthType* widthPtr;

	unsigned int bufferNumber;

	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(world);
	tview = XMMatrixTranspose(view);
	tproj = XMMatrixTranspose(projection);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(weightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	weightPtr = (WeightBufferType*)mappedResource.pData;

	// Copy the matrices into the weight buffer for the pixel shader.
	weightPtr->weight0 = weightings.x;// worldMatrix;
	weightPtr->weight1 = weightings.y;
	weightPtr->weight2 = weightings.z;
	weightPtr->padding = 0.0f;

	deviceContext->Unmap(weightBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;
	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &weightBuffer);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;

	deviceContext->Unmap(matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;
    // Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(ScreenWidthBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);

	widthPtr = (ScreenWidthType*)mappedResource2.pData;
	widthPtr->ScreenWidth = screenWidth;
	widthPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	deviceContext->Unmap(ScreenWidthBuffer, 0);

	bufferNumber = 1;
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &ScreenWidthBuffer);

	
}

void HorizontalBlurShader::initShader(WCHAR *vsFilename, WCHAR *psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC screenWidthBufferDesc;
	D3D11_BUFFER_DESC weightBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;

	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);



	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	screenWidthBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	screenWidthBufferDesc.ByteWidth = sizeof(ScreenWidthType);
	screenWidthBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	screenWidthBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	screenWidthBufferDesc.MiscFlags = 0;
	screenWidthBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&screenWidthBufferDesc, NULL, &ScreenWidthBuffer);



	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	weightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	weightBufferDesc.ByteWidth = sizeof(WeightBufferType);
	weightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	weightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	weightBufferDesc.MiscFlags = 0;
	weightBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&weightBufferDesc, NULL, &weightBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 3;
	samplerDesc.BorderColor[1] = 3;
	samplerDesc.BorderColor[2] = 3;
	samplerDesc.BorderColor[3] = 3;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);
}

void HorizontalBlurShader::render(ID3D11DeviceContext* deviceContext, int vertexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	// Base render function.
	BaseShader::render(deviceContext, vertexCount);
}
