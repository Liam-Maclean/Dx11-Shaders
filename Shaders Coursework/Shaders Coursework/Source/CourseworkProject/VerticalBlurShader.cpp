#include "VerticalBlurShader.h"

VerticalBlurShader::VerticalBlurShader(ID3D11Device * device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"verticalblur_vs.cso", L"verticalblur_ps.cso");
}

VerticalBlurShader::~VerticalBlurShader()
{

}

void VerticalBlurShader::setShaderParameters(ID3D11DeviceContext * deviceContext, const XMMATRIX & world, const XMMATRIX & view, const XMMATRIX & projection, float screenHeight, ID3D11ShaderResourceView* texture, XMFLOAT3 weightings)
{
    HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	weightBufferType* weightPtr;
	screenHeightType* heightPtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(world);
	tview = XMMatrixTranspose(view);
	tproj = XMMatrixTranspose(projection);


	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(weightingsBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	weightPtr = (weightBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	weightPtr->weight0 = weightings.x;// worldMatrix;
	weightPtr->weight1 = weightings.y;
	weightPtr->weight2 = weightings.z;
	weightPtr->padding = 0.0f;
	// Unlock the constant buffer.
	deviceContext->Unmap(weightingsBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;
	// Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &weightingsBuffer);



	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	// Get a pointer to the data in the constant buffer.
	dataPtr = (MatrixBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	// Unlock the constant buffer.
	deviceContext->Unmap(matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;
    // Now set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &matrixBuffer);

	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(screenHeightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	heightPtr = (screenHeightType*)mappedResource.pData;
	heightPtr->screenHeight = screenHeight;
	heightPtr->padding = XMFLOAT3(0.0f, 0.0f, 0.0f);

	deviceContext->Unmap(screenHeightBuffer, 0);

	bufferNumber = 1;

	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &screenHeightBuffer);

	
}

void VerticalBlurShader::initShader(WCHAR * vsFilename, WCHAR *psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC screenHeightBufferDesc;
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
	weightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	weightBufferDesc.ByteWidth = sizeof(weightBufferType);
	weightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	weightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	weightBufferDesc.MiscFlags = 0;
	weightBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&weightBufferDesc, NULL, &weightingsBuffer);
	

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	screenHeightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	screenHeightBufferDesc.ByteWidth = sizeof(screenHeightType);
	screenHeightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	screenHeightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	screenHeightBufferDesc.MiscFlags = 0;
	screenHeightBufferDesc.StructureByteStride = 0;

	renderer->CreateBuffer(&screenHeightBufferDesc, NULL, &screenHeightBuffer);


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

void VerticalBlurShader::render(ID3D11DeviceContext* deviceContext, int vertexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);

	// Base render function.
	BaseShader::render(deviceContext, vertexCount);
};
