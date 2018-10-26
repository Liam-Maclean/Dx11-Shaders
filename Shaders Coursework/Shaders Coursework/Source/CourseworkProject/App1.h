// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#define _XM_NO_INTRINSICS_
#include "../DXFramework/DXF.h"
#include "ColourShader.h"
#include "LightShader.h"
#include "TextureShader.h"
#include "HorizontalBlurShader.h"
#include "VerticalBlurShader.h"
#include "VolumetricShader.h"
#include "GeometryShader.h"
#include "SinVertexShader.h"
#include "TessellationShader.h"
#include "CelShader.h"
#include "SilhouetteShader.h"
#include "TessSphereMesh.h"
#include "SpotLightShader.h"
#include "DepthShader.h"
#include "ShadowShader.h"


#pragma once
class App1 : public BaseApplication
{
public:

	//selected mesh enum for selecting mesh (polymorphism)
	enum SelectedMesh
	{
		plane,
		triangle,
		cube,
		quad,
		sphere,
		teapot,
		drone
	};

	//selected Tess mesh enum for selecting what tessmesh to render
	enum SelectedTessMesh
	{
		tessTriangle,
		tessSphere
	};


	App1();
	~App1();

	//Initialise function for base application
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in);

	//update
	bool frame();

protected:
	//========blur functions=======
	void RenderSceneWithBlur();
	void RenderToTexture();
	void DownSample();
	void HorizontalBlur();
	void VerticalBlur();
	void UpSample();
	void RenderScene();

	//========Light Functions=======
	void RenderSceneWithLights();
	void RenderSceneWithSpotLights();
	void RenderSceneWithCelShader();
	void VolumetricLighting();
	//======Shadows========
	void RenderToTextureShadows();
	void RenderSceneWithDepthShadows();


	//========Vertex Manipulation Functions=======
	void RenderSceneWithVertexManipulation();
	void RenderSceneWithTesselation();

	//========Gui Functions========
	void gui();
	void BlurGUI();
	void LightGUI();
	void VertexManipGUI();
	void TessellationGUI();
	void SpotLightGUI();
	void CelGUI();
	void ShadowGUI();



	bool render();

private:

	int _screenWidth, _screenHeight;
	SelectedMesh selectedMesh;
	SelectedTessMesh selectedTessMesh;
	DirectX::XMMATRIX scalingMatrix;

	OrthoMesh* orthoMesh;
	float tempTimer;
	float speed, height;
	//===========================IMGUI Variables=============================
	float imGUIHorizontalWeightings[3] = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 horzWeightings;

	float imGUIVerticalWeightings[3] = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 vertWeightings;

	float imGUIOutlineColour[3] = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4 outlineColor;

	float imGUIAmbientColor[3] = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4 ambientColor;

	float imGUIColour[3] = { 0.0f, 0.0f, 0.0f };
	float imGUIColour2[3] = { 0.0f, 0.0f, 0.0f };
	float imGUIColour3[3] = { 0.0f, 0.0f, 0.0f };
	float imGUIColour4[3] = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 lightColour[4];

	float imGUIBackgroundColour[3] = { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 backgroundColour = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	float imGUILightPosition[3] = { 0.0f, 0.0f, 0.0f };
	float imGUILightPosition2[3] = { 0.0f, 0.0f, 0.0f };
	float imGUILightPosition3[3] = { 0.0f, 0.0f, 0.0f };
	float imGUILightPosition4[3] = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 lightPosition[4];

	float imGUILightDirection[3] = { 0.0f, 0.0f, 0.0f };
	float imGUILightDirection2[3] = { 0.0f, 0.0f, 0.0f };
	float imGUILightDirection3[3] = { 0.0f, 0.0f, 0.0f };
	float imGUILightDirection4[3] = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 lightDirection[4];

	float imGUIShadowLightPos[3] = { -5.0f, 0.0f, -5.0f };
	DirectX::XMFLOAT3 shadowLightPos = XMFLOAT3(-5.0f, 0.0f, -5.0f);

	DirectX::XMFLOAT3 shadowLightLookat;

	bool bLightShader = true, bBlurShader, bSinShader, bTesselationShader, bCelShader, bSpotLightShader, bShadowShader;

	//=========Spot Light variables===========
	float imGUISpotLightColor[4] = { 0.8f, 0.5f, 0.0f, 1.0f };
	DirectX::XMFLOAT4 spotLightColor = XMFLOAT4(0.8f, 0.5f, 0.0f, 1.0f);

	float imGUISpotLightTargetPosition[3] = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 spotLightTarget = XMFLOAT3(0.0f, 0.0f, 0.0f);

	float imGUISpotLightPosition[3] = { 3.0f, 3.0f, 0.0f };
	DirectX::XMFLOAT3 spotLightPosition = XMFLOAT3(3.0f, 3.0f, 0.0f);


	vector<Light*> m_light;

	float imGUISpotLightAttenuation[1] = { 1.0f };
	float spotLightAttenuation = 1.0f;

	float imGUISpotLightRange[1] = { 10.0f };
	float spotLightRange = 10.0f;


	SphereMesh* spotLightMesh;

	PointMesh* spotLightDirectionWidget;
	Light* spotLight;
	//==========================================

	Light* shadowLight;


	//Matrices
	DirectX::XMMATRIX spotLightWorldMatrix;
	DirectX::XMMATRIX finalSpotLightWorldMatrix;
	DirectX::XMMATRIX lightViewMatrix;
	DirectX::XMMATRIX lightProjectionMatrix;
	DirectX::XMMATRIX worldMatrixLight[4];
	DirectX::XMMATRIX finalMatrixCamera[4];
	DirectX::XMMATRIX floorWorldMatrix;

	bool wireFrameMode = false;

	XMFLOAT4X4 g_worldLightMatrix;




	//=============render textures=================
	RenderTexture* renderTexture;
	RenderTexture* normalScene;
	RenderTexture* volumetricTexture;
	RenderTexture* downSampleTexture;
	RenderTexture* downSampleTargetWindowHorz;
	RenderTexture* downSampleTargetWindowVert;
	RenderTexture* upSample;
	RenderTexture* finalRenderTexture;
	RenderTexture* renderTextureShadow;



	//=============Shader CPPs===============
	CelShader* celShader;
	TextureShader* textureShader;
	VolumetricShader* volumetricShader;
	VerticalBlurShader* verticalBlurShader;
	HorizontalBlurShader* horizontalBlurShader;
	LightShader* lightShader;
	GeometryShader* geometryShader;
	ColourShader* colourShader;
	SinVertexShader* sinVertexShader;
	TessellationShader* tessellationShader;
	SilhouetteShader* silhouetteShader;
	SpotLightShader* spotLightShader;
	DepthShader* depthShader;
	ShadowShader* shadowShader;


	//===============MODELS====================
	SphereMesh* lightSphere[4];
	PointMesh* lightDirectionWidget[4];
	BaseMesh* chosenMesh;
	BaseMesh* chosenMesh2;
	BaseMesh* chosenTessMesh;
	SphereMesh* silhouetteMesh;
	SphereMesh* sphereMesh;
	PlaneMesh* floorMesh;
	Model* droneModel;
	Model* teapotModel;
	SphereMesh* chosenSphere;
	CubeMesh* chosenCube;
	TriangleMesh* chosenTriangle;
	PlaneMesh* chosenPlane;
	QuadMesh* chosenQuad;
	TessellationMesh* tessTriangleMesh;
	TessSphereMesh* tessSphereMesh;
};

#endif