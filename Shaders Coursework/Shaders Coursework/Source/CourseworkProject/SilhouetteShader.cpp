// texture shader.cpp
#include "SilhouetteShader.h"


SilhouetteShader::SilhouetteShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"silhouette_vs.cso", L"silhouette_ps.cso");
}


SilhouetteShader::~SilhouetteShader()
{
	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	//Release base shader components
	BaseShader::~BaseShader();
}


void SilhouetteShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_BUFFER_DESC colourBufferDesc;
	D3D11_RASTERIZER_DESC rasterDesc;

	


	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);


	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = true;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;
	renderer->CreateRasterizerState(&rasterDesc, &rasterBuffer);



	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	colourBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	colourBufferDesc.ByteWidth = sizeof(colorBufferType);
	colourBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	colourBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	colourBufferDesc.MiscFlags = 0;
	colourBufferDesc.StructureByteStride = 0;


	renderer->CreateBuffer(&colourBufferDesc, NULL, &colorBuffer);



}


void SilhouetteShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &worldMatrix, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, XMFLOAT4 color)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;

	// Transpose the matrices to prepare them for the shader.
	tworld = DirectX::XMMatrixTranspose(worldMatrix);
	tview = DirectX::XMMatrixTranspose(viewMatrix);
	tproj = DirectX::XMMatrixTranspose(projectionMatrix);

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


	//Additional
	// Send light data to pixel shader
	deviceContext->Map(colorBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	colorPtr = (colorBufferType*)mappedResource.pData;
	colorPtr->color = color;
	deviceContext->Unmap(colorBuffer, 0);
	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &colorBuffer);
}


void SilhouetteShader::render(ID3D11DeviceContext* deviceContext, int indexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	deviceContext->RSSetState(rasterBuffer);

	// Base render function.
	BaseShader::render(deviceContext, indexCount);

	deviceContext->RSSetState(NULL);
}
