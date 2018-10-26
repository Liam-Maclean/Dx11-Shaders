#include "TessSphereMesh.h"



TessSphereMesh::TessSphereMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution): SphereMesh(device, deviceContext, lresolution)
{

}


TessSphereMesh::~TessSphereMesh()
{
}

void TessSphereMesh::sendData(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	// Set the type of primitive that should be rendered from this vertex buffer, in this case control patch for tessellation.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);
}
