#include "VolumetricShader.h"





VolumetricShader::VolumetricShader(ID3D11Device * device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"volumetriclighting_vs.cso", L"volumetriclighting_ps.cso");
}

VolumetricShader::~VolumetricShader()
{

}

void VolumetricShader::initShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_BUFFER_DESC lightBufferDesc;
	D3D11_BLEND_DESC blendDesc;
	
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


	//ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
	//blendDesc.RenderTarget[0].BlendEnable = true;
	//blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//renderer->CreateBlendState(&blendDesc, &BlendState);



	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	renderer->CreateSamplerState(&samplerDesc, &sampleState);


	// Setup light buffer
	// Setup the description of the light dynamic constant buffer that is in the pixel shader.
	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

}

void VolumetricShader::setShaderParameters(ID3D11DeviceContext * deviceContext, const XMMATRIX & world, const XMMATRIX & view, const XMMATRIX & projection, const XMMATRIX &orthoView, const XMMATRIX &orthoMatrix, ID3D11ShaderResourceView * texture, Light * light)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;
	LightBufferType* lightPtr;
	unsigned int bufferNumber;
	XMMATRIX tworld, tview, tproj;
	XMFLOAT2 lightScreenPosition;

	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(world);
	tview = XMMatrixTranspose(orthoView);
	tproj = XMMatrixTranspose(orthoMatrix);

	//deviceContext->OMSetBlendState(BlendState, 0, 0xffffffff);


	XMVECTOR worldCoord = XMLoadFloat3(&light->getPosition());
	XMVECTOR ScreenVector = WorldToScreenPoint(worldCoord, view, projection, world, 0, 0);
	XMStoreFloat2(&lightScreenPosition, ScreenVector);


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
	deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	lightPtr = (LightBufferType*)mappedResource.pData;
	lightPtr->diffuse = light->getDiffuseColour();
	lightPtr->screenPos = lightScreenPosition;
	lightPtr->decay = 0.f;
	lightPtr->exposure  = 0.5f;
	lightPtr->lightDirection = light->getDirection();
	lightPtr->weight = 1.0;

	deviceContext->Unmap(lightBuffer, 0);
	bufferNumber = 0;
	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &lightBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);



}


void VolumetricShader::render(ID3D11DeviceContext * deviceContext, int vertexCount)
{
	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &sampleState);
	//deviceContext->OMSetBlendState(BlendState, 0, 0xffffffff);
	// Base render function.
	BaseShader::render(deviceContext, vertexCount);
}

DirectX::XMVECTOR VolumetricShader::WorldToScreenPoint(XMVECTOR Point2D, const XMMATRIX & viewMatrix, const XMMATRIX & projectionMatrix, const XMMATRIX &worldMatrix, int width, int height)
{ 
	XMVECTOR position = Point2D;

	position = XMVector3Project(position, 0, 0, 1024, 576, 0.0f, 1.0f, projectionMatrix, viewMatrix, worldMatrix);

	return position;
}

