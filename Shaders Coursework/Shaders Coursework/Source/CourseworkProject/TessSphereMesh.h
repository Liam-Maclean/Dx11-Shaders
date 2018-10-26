#pragma once
#include "../DXFramework/SphereMesh.h"
class TessSphereMesh :
	public SphereMesh
{
public:
	TessSphereMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int lresolution);
	~TessSphereMesh();

	void sendData(ID3D11DeviceContext* deviceContext);
};

