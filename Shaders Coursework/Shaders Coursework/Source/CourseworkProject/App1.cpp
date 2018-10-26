// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	//BaseApplication::BaseApplication();
	colourShader = nullptr;
}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input *in)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in);
	_screenWidth = screenWidth;
	_screenHeight = screenHeight;
	scalingMatrix = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
	selectedMesh = SelectedMesh::cube;

	floorWorldMatrix = renderer->getWorldMatrix();
	floorWorldMatrix = DirectX::XMMatrixTranslation(-10, -2, -10); 

	//==========Light initialisation=========
	for (int i = 0; i < 4; i++)
	{
		m_light.push_back(new Light());
		m_light[i]->setPosition(lightPosition[i].x, lightPosition[i].y, lightPosition[i].z);
		m_light[i]->setDiffuseColour(lightColour[i].x, lightColour[i].y, lightColour[i].z, 1.0f);
		m_light[i]->setAmbientColour(0.6f, 0.6f, 0.6f, 1.0f);
		m_light[i]->setDirection(0.0f, 0.0f, 0.0f);
	}


	m_light[0]->setLookAt(0.0f, 0.0f, 0.0f);
	//Spot Light
	spotLight = new Light();
	spotLight->setPosition(spotLightPosition.x, spotLightPosition.z, spotLightPosition.z);
	spotLight->setDiffuseColour(spotLightColor.x, spotLightColor.y, spotLightColor.z, spotLightColor.z);

	spotLightWorldMatrix = renderer->getWorldMatrix();
	spotLightWorldMatrix = DirectX::XMMatrixTranslation(spotLightPosition.x, spotLightPosition.y, spotLightPosition.z);

	spotLightMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	spotLightDirectionWidget = new PointMesh(renderer->getDevice(), renderer->getDeviceContext());



	//==============Shadow Light initialisation===========
	shadowLightLookat = XMFLOAT3(0.0f, 0.0f, 0.0f);
	shadowLight = new Light();
	shadowLight->setPosition(2.0f, 3.0f, 0.0f);
	shadowLight->setAmbientColour(0.1f, 0.1f, 0.1f, 1.0f);
	shadowLight->setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
	shadowLight->setDirection(0.0f, -1.0f, 0.0f);
	shadowLight->setLookAt(shadowLightLookat.x, shadowLightLookat.y, shadowLightLookat.z);
	shadowLight->generateOrthoMatrix(screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);




	//==========Texture Loading==========
	textureMgr->loadTexture("Default", L"../res/brick1.dds");
	textureMgr->loadTexture("Bunny", L"../res/bunny.png");

	//==========Shaders==============	
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	volumetricShader = new VolumetricShader(renderer->getDevice(), hwnd);
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
	lightShader = new LightShader(renderer->getDevice(), hwnd);
	colourShader = new ColourShader(renderer->getDevice(), hwnd);
	geometryShader = new GeometryShader(renderer->getDevice(), hwnd);
	sinVertexShader = new SinVertexShader(renderer->getDevice(), hwnd);
	tessellationShader = new TessellationShader(renderer->getDevice(), hwnd);
	celShader = new CelShader(renderer->getDevice(), hwnd);
	silhouetteShader = new SilhouetteShader(renderer->getDevice(), hwnd);
	spotLightShader = new SpotLightShader(renderer->getDevice(), hwnd);
	shadowShader = new ShadowShader(renderer->getDevice(), hwnd);
	depthShader = new DepthShader(renderer->getDevice(), hwnd);



	//============OrthoMesh and Render to texture=============
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight, 0, 0);

	//============Blur render-to-textures=============
	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	downSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	downSampleTargetWindowHorz = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	downSampleTargetWindowVert = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	upSample = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	renderTextureShadow = new RenderTexture(renderer->getDevice(), 1024, 1024, SCREEN_NEAR, SCREEN_DEPTH);


	//============Geometry=============
	chosenSphere = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	chosenCube = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext());
	chosenQuad = new QuadMesh(renderer->getDevice(), renderer->getDeviceContext());
	chosenTriangle = new TriangleMesh(renderer->getDevice(), renderer->getDeviceContext());
	chosenPlane = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	tessTriangleMesh = new TessellationMesh(renderer->getDevice(), renderer->getDeviceContext());
	tessSphereMesh = new TessSphereMesh(renderer->getDevice(), renderer->getDeviceContext(),20);
	silhouetteMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	sphereMesh = new SphereMesh(renderer->getDevice(), renderer->getDeviceContext());
	floorMesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());
	droneModel = new Model(renderer->getDevice(), renderer->getDeviceContext(), "../res/drone.obj");
	teapotModel = new Model(renderer->getDevice(), renderer->getDeviceContext(), "../res/teapot.obj");


	chosenTessMesh = tessSphereMesh;
	chosenMesh = chosenSphere;


}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	if (colourShader)
	{
		delete colourShader;
		colourShader = 0;
	}
}

//Update (override with base frame)
bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();

	//update lights with variables from imGUI
	m_light[0]->setAmbientColour(ambientColor.x, ambientColor.y, ambientColor.z, ambientColor.w);
	spotLight->setAmbientColour(ambientColor.x, ambientColor.y, ambientColor.z, ambientColor.w);
	shadowLight->setPosition(shadowLightPos.x, shadowLightPos.y, shadowLightPos.z);


	//Switch Mesh dependant on choice.
	switch (selectedMesh)
	{
		//switch to cube
	case SelectedMesh::cube:
		chosenMesh = chosenCube;
		chosenMesh2 = chosenCube;
		break;
		//switch to sphere
	case SelectedMesh::sphere:
		chosenMesh = chosenSphere;
		chosenMesh2 = chosenSphere;
		break;
		//switch to plane
	case SelectedMesh::plane:
		chosenMesh = chosenPlane;
		chosenMesh2 = chosenPlane;
		break;
		//switch to triangle
	case SelectedMesh::triangle:
		chosenMesh = chosenTriangle;
		chosenMesh2 = chosenTriangle;
		break;
		//switch to quad
	case SelectedMesh::quad:
		chosenMesh = chosenQuad;
		chosenMesh2 = chosenQuad;
		break;
		//switch to drone
	case SelectedMesh::drone:
		chosenMesh = droneModel;
		chosenMesh2 = droneModel;
		break;
		//switch to teapot
	case SelectedMesh::teapot:
		chosenMesh = teapotModel;
		chosenMesh2 = teapotModel;
		break;
	}


	switch (selectedTessMesh)
	{
	case SelectedTessMesh::tessSphere:
		chosenTessMesh = tessSphereMesh;
		break;
	case SelectedTessMesh::tessTriangle:
		chosenTessMesh = tessTriangleMesh;
	}


	//update the lights position and matrices
	for (int i = 0; i < 4; i++)
	{
		m_light[i]->setDiffuseColour(lightColour[i].x, lightColour[i].y, lightColour[i].z, 1.0f);
		m_light[i]->setPosition(lightPosition[i].x, lightPosition[i].y, lightPosition[i].z);
		m_light[i]->setDirection(lightDirection[i].x, lightDirection[i].y, lightDirection[i].z);
	}
	spotLight->setPosition(spotLightPosition.x, spotLightPosition.y, spotLightPosition.z);
	spotLight->setDiffuseColour(spotLightColor.x, spotLightColor.y, spotLightColor.z, 1.0f);
	spotLightWorldMatrix = DirectX::XMMatrixTranslation(spotLight->getPosition().x, spotLight->getPosition().y, spotLight->getPosition().z);
	finalSpotLightWorldMatrix = DirectX::XMMatrixMultiply(scalingMatrix, spotLightWorldMatrix);
	

	if (!result)
	{
		return false;
	}

	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

//Render (override)
bool App1::render()
{
	//toggle wireframe mode
	renderer->setWireframeMode(wireFrameMode);
	
	//if blurshader is active
	if (bBlurShader)
	{
		//blur shader
		RenderSceneWithBlur();
	}
	//if light shader is active
	else if (bLightShader)
	{
		//light shader
		RenderSceneWithLights();
	}
	//if spotlight shader is active
	else if (bSpotLightShader)
	{
		//spot light shader
		RenderSceneWithSpotLights();
	}
	//if vertex manip shader is active
	else if (bSinShader)
	{
		//vertex manip shader
		RenderSceneWithVertexManipulation();
	}
	//if tessellation shader is active
	else if (bTesselationShader)
	{
		//tessellation shader
		RenderSceneWithTesselation();
	}
	//if cel shader is active
	else if (bCelShader)
	{
		//cel shader
		RenderSceneWithCelShader();
	}
	//if shadow shader is active
	else if (bShadowShader)
	{
		//shadow shader
		RenderToTextureShadows();
		RenderSceneWithDepthShadows();
	}
	return true;
}

//Cel Shading
void App1::RenderSceneWithCelShader()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	renderer->beginScene(backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);

	camera->update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//outline shader
	chosenMesh->sendData(renderer->getDeviceContext());
	silhouetteShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, outlineColor);
	silhouetteShader->render(renderer->getDeviceContext(), chosenMesh->getIndexCount());


	//Cel shader (colour in front)
	chosenMesh2->sendData(renderer->getDeviceContext());
	celShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("Default"), m_light[0]);
	celShader->render(renderer->getDeviceContext(), chosenMesh2->getIndexCount());

	//render GUI
	gui();
	renderer->endScene();
}

//Direction Lights Render
void App1::RenderSceneWithLights()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	renderer->beginScene(backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);

	camera->update();

	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	
	//Render Chosen mesh with light shader passing in the light
	chosenMesh->sendData(renderer->getDeviceContext());
	lightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("Default"), m_light);
	lightShader->render(renderer->getDeviceContext(), chosenMesh->getIndexCount());


	//render GUI
	gui();
	renderer->endScene();

}

//Spot Lights Render
void App1::RenderSceneWithSpotLights()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	renderer->beginScene(backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);

	camera->update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//Render mesh to be lit with spotlight shader
	chosenMesh->sendData(renderer->getDeviceContext());
	spotLightShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("Default"), spotLight, spotLightAttenuation, spotLightRange, spotLightTarget);
	spotLightShader->render(renderer->getDeviceContext(), chosenMesh->getIndexCount());

	//create physical sphere to simulate where light is in 3D space.
	spotLightMesh->sendData(renderer->getDeviceContext());
	colourShader->setShaderParameters(renderer->getDeviceContext(), finalSpotLightWorldMatrix, viewMatrix, projectionMatrix, XMFLOAT4(spotLightColor.x, spotLightColor.y, spotLightColor.z, 1.0f));
	colourShader->render(renderer->getDeviceContext(), spotLightMesh->getIndexCount());

	//Render 3D widget using geometry shader to show light direction
	spotLightDirectionWidget->sendData(renderer->getDeviceContext());
	geometryShader->setShaderParameters(renderer->getDeviceContext(), finalSpotLightWorldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("Default"), spotLight, spotLightTarget, worldMatrix);
	geometryShader->render(renderer->getDeviceContext(), spotLightDirectionWidget->getIndexCount());


	//render GUI
	gui();
	renderer->endScene();

}

//Blur post processing Render
void App1::RenderSceneWithBlur()
{
	//*Blur*
	RenderToTexture();
	DownSample();
	HorizontalBlur();
	VerticalBlur();
	UpSample();
	RenderScene();
}

//Vertex Manipulation Render
void App1::RenderSceneWithVertexManipulation()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	renderer->beginScene(backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);

	camera->update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//increment time variable with deltatime
	tempTimer += timer->getTime();

	//render chosen mesh passing in delta time, speed and height variables
	chosenMesh->sendData(renderer->getDeviceContext());
	sinVertexShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, tempTimer, speed, height);
	sinVertexShader->render(renderer->getDeviceContext(), chosenMesh->getIndexCount());

	//render GUI
	gui();
	renderer->endScene();
}

//Tesselation Render
void App1::RenderSceneWithTesselation()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	renderer->beginScene(backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);

	camera->update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//render chosen tessellatable mesh tessellating 4 times
	chosenTessMesh->sendData(renderer->getDeviceContext());
	tessellationShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("Default"));
	tessellationShader->render(renderer->getDeviceContext(), chosenTessMesh->getIndexCount());

	//render GUI
	gui();
	renderer->endScene();
}

//Render to texture for shadows
void App1::RenderToTextureShadows()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	//Set render target to current screen
	renderTextureShadow->setRenderTarget(renderer->getDeviceContext());

	//make sure screen is black for depth testing 
	renderTextureShadow->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	camera->update();

	//create light matrices for depth testing
	shadowLight->generateViewMatrix();
	shadowLight->generateProjectionMatrix(SCREEN_NEAR, SCREEN_DEPTH);
	shadowLight->generateOrthoMatrix(_screenWidth, _screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	lightViewMatrix = shadowLight->getViewMatrix();
	lightProjectionMatrix = shadowLight->getProjectionMatrix();
	worldMatrix = renderer->getWorldMatrix();



	//render with depth shader using light matrices
	chosenMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), chosenMesh->getIndexCount());

	//render with depth shader using light matrices
	floorMesh->sendData(renderer->getDeviceContext());
	depthShader->setShaderParameters(renderer->getDeviceContext(), floorWorldMatrix, lightViewMatrix, lightProjectionMatrix);
	depthShader->render(renderer->getDeviceContext(), floorMesh->getIndexCount());

	//set render target back to the backbuffer and reset the viewport from light matrices.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

//Render Scene for shadows
void App1::RenderSceneWithDepthShadows()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoProjection;
	renderer->beginScene(ambientColor.x, ambientColor.y, ambientColor.z, 1.0f);

	camera->update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	//create matrices again for accurate locations
	shadowLight->generateViewMatrix();
	shadowLight->generateProjectionMatrix(SCREEN_NEAR, SCREEN_DEPTH);
	shadowLight->generateOrthoMatrix(_screenWidth, _screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	lightViewMatrix = shadowLight->getViewMatrix();
	lightProjectionMatrix = shadowLight->getProjectionMatrix();



	//render floor mesh using default texture and shadow map as parameters
	floorMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), floorWorldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("Default"), renderTextureShadow->getShaderResourceView(), shadowLight);
	shadowShader->render(renderer->getDeviceContext(), floorMesh->getIndexCount());

	//render chosen Mesh using default texture and shadowmap as parameters
	chosenMesh->sendData(renderer->getDeviceContext());
	shadowShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("Default"), renderTextureShadow->getShaderResourceView(), shadowLight);
	shadowShader->render(renderer->getDeviceContext(), chosenMesh->getIndexCount());

	orthoViewMatrix = camera->getOrthoViewMatrix();
	orthoProjection = renderer->getOrthoMatrix();


	//render GUI 
	gui();
	renderer->endScene();

}


//GUI stuff
void App1::gui()
{
	// Force turn off on Geometry shader
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	//directional light button
	if (ImGui::Button("Directional Light Shader", ImVec2(200, 40)))
	{
		bSinShader = false;
		bBlurShader = false;
		bCelShader = false;
		bTesselationShader = false;
		bSpotLightShader = false;
		bLightShader = true;
		selectedMesh = SelectedMesh::sphere;
	}

	//Spot light button
	if (ImGui::Button("Spot Light Shader", ImVec2(200, 40)))
	{
		bSinShader = false;
		bBlurShader = false;
		bCelShader = false;
		bTesselationShader = false;
		bLightShader = false;
		bSpotLightShader = true;

		selectedMesh = SelectedMesh::sphere;
	}

	//Blur button
	if (ImGui::Button("Blur Shader", ImVec2(200, 40)))
	{
		bTesselationShader = false;
		bSinShader = false;
		bCelShader = false;
		bLightShader = false;
		bShadowShader = false;
		bSpotLightShader = false;
		bBlurShader = true;
		selectedMesh = SelectedMesh::sphere;
	}

	//Vertex manip button
	if (ImGui::Button("Vertex Manipulation Shader", ImVec2(200, 40)))
	{
		bTesselationShader = false;
		bLightShader = false;
		bCelShader = false;
		bBlurShader = false;
		bSpotLightShader = false;
		bShadowShader = false;
		bSinShader = true;
		selectedMesh = SelectedMesh::sphere;
	}

	//Tessellation button
	if (ImGui::Button("Tesselation Shader", ImVec2(200, 40)))
	{
		bLightShader = false;
		bCelShader = false;
		bBlurShader = false;
		bSinShader = false;
		bShadowShader = false;
		bSpotLightShader = false;
		bTesselationShader = true;
		selectedMesh = SelectedMesh::sphere;
	}

	//Cel shader button
	if (ImGui::Button("Cel Shader", ImVec2(200, 40)))
	{
		bLightShader = false;
		bBlurShader = false;
		bSinShader = false;
		bTesselationShader = false;
		bSpotLightShader = false;
		bShadowShader = false;
		bCelShader = true;
		selectedMesh = SelectedMesh::sphere;
	}

	//Shadow button
	if (ImGui::Button("Shadow Shader", ImVec2(200, 40)))
	{
		bLightShader = false;
		bBlurShader = false;
		bSinShader = false;
		bTesselationShader = false;
		bSpotLightShader = false;
		bCelShader = false;
		bShadowShader = true;
		selectedMesh = SelectedMesh::sphere;
	}


	//Togglable wireframe mode
	ImGui::Checkbox("Wireframe Mode", &wireFrameMode);


	//if Blur Shader is on, activate blur GUI widgets
	if (bBlurShader)
	{
		BlurGUI();
	}
	//if Light Shader is on, activate Light Shader Widgets.
	if (bLightShader)
	{
		LightGUI();
	}
	//if Sin Shader is on, activate sin shader widgets
	if (bSinShader)
	{
		VertexManipGUI();
	}
	//if tessellation shader is on, activate tessellation shader widgets
	if (bTesselationShader)
	{
		TessellationGUI();
	}
	//if spotlight shader is on, activate spotlight shader widgets
	if (bSpotLightShader)
	{
		SpotLightGUI();
	}
	//if cel shader is on, activate cel shader widgets
	if (bCelShader)
	{
		CelGUI();
	}
	//if shadow shader is on, activate cel shader widgets
	if (bShadowShader)
	{
		ShadowGUI();
	}
	//show FPS
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::PushButtonRepeat(true);

	// Render UI
	ImGui::Render();
}

//Spot Light Shader GUI
void App1::SpotLightGUI()
{
	const char* listbox_items[] = { "Cube", "Sphere", "Plane", "Triangle", "QuadMesh" };
	static int listbox_items_current = 1;
	ImGui::LabelText("Light Shader", "CURRENLY ON: ");

	//Shape selection list 
	if (ImGui::ListBox("Shape Selection", &listbox_items_current, listbox_items, 5))
	{
		switch (listbox_items_current)
		{
		case 0:
			//change shape
			selectedMesh = SelectedMesh::cube;
			break;
		case 1:
			//change shape
			selectedMesh = SelectedMesh::sphere;
			break;
		case 2:
			//change shape
			selectedMesh = SelectedMesh::plane;
			break;
		case 3:
			//change shape
			selectedMesh = SelectedMesh::triangle;
			break;
		case 4:
			//change shape
			selectedMesh = SelectedMesh::quad;
			break;
		case 5:
			break;
		}
	}

	//backgroudn color widget
	if (ImGui::ColorEdit3("Background color", imGUIBackgroundColour)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		backgroundColour.x = (imGUIBackgroundColour[0]);
		backgroundColour.y = (imGUIBackgroundColour[1]);
		backgroundColour.z = (imGUIBackgroundColour[2]);
	}

	//ambient color widget
	if (ImGui::ColorEdit3("Ambient color", imGUIAmbientColor)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		ambientColor.x = (imGUIAmbientColor[0]);
		ambientColor.y = (imGUIAmbientColor[1]);
		ambientColor.z = (imGUIAmbientColor[2]);
		ambientColor.w = 1.0f;
	}


	ImGui::CollapsingHeader("Spot Light");

	//spotlight float slider
	if (ImGui::SliderFloat("Spot Light Attenuation",&spotLightAttenuation, 0.0f, 1.0f)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		//spotLightRange = imGUISpotLightRange[0];
	}

	//spotlight range float
	if (ImGui::InputFloat("Spot Light Range", imGUISpotLightRange)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		spotLightRange = imGUISpotLightRange[0];
	}

	//spotlight position widget
	if (ImGui::InputFloat3("Spot Light Position", imGUISpotLightPosition)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		spotLightPosition.x = (imGUISpotLightPosition[0]);
		spotLightPosition.y = (imGUISpotLightPosition[1]);
		spotLightPosition.z = (imGUISpotLightPosition[2]);
	}

	//spotlight target widget
	if (ImGui::InputFloat3("Spot Light Target", imGUISpotLightTargetPosition)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		spotLightTarget.x = (imGUISpotLightTargetPosition[0]);
		spotLightTarget.y = (imGUISpotLightTargetPosition[1]);
		spotLightTarget.z = (imGUISpotLightTargetPosition[2]);
	}

	//spotlight diffuse widget
	if (ImGui::InputFloat3("Spot Light Diffuse", imGUISpotLightColor)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		spotLightColor.x = (imGUISpotLightColor[0]);
		spotLightColor.y = (imGUISpotLightColor[1]);
		spotLightColor.z = (imGUISpotLightColor[2]);
	}

}

//Cel Shader GUI
void App1::CelGUI()
{


	const char* listbox_items[] = {  "Sphere", "drone", "teapot"};
	static int listbox_items_current = 1;
	ImGui::LabelText("Cel Shader", "CURRENLY ON: ");

	//shape selection list
	if (ImGui::ListBox("Shape Selection", &listbox_items_current, listbox_items, 3))
	{
		switch (listbox_items_current)
		{
		case 0:
			//change shape
			selectedMesh = SelectedMesh::sphere;
			break;
		case 1:
			//change shape
			selectedMesh = SelectedMesh::drone;
			break;
		case 2:
			//change shape
			selectedMesh = SelectedMesh::teapot;
			break;
		}
	}

	//background color widget
	if (ImGui::ColorEdit3("Background color", imGUIBackgroundColour)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		backgroundColour.x = (imGUIBackgroundColour[0]);
		backgroundColour.y = (imGUIBackgroundColour[1]);
		backgroundColour.z = (imGUIBackgroundColour[2]);
		outlineColor.w = 1.0f;
	}

	ImGui::CollapsingHeader("Directional Light: ");


	//outline color widget
	if (ImGui::InputFloat3("Outline Color", imGUIOutlineColour)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		outlineColor.x = (imGUIOutlineColour[0]);
		outlineColor.y = (imGUIOutlineColour[1]);
		outlineColor.z = (imGUIOutlineColour[2]);
	}

	//light direction widget
	if (ImGui::InputFloat3("Light Direction", imGUILightDirection)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		lightDirection[0].x = (imGUILightDirection[0]);
		lightDirection[0].y = (imGUILightDirection[1]);
		lightDirection[0].z = (imGUILightDirection[2]);
	}

	//light color widget
	if (ImGui::InputFloat3("Light color", imGUIColour)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		lightColour[0].x = (imGUIColour[0]);
		lightColour[0].y = (imGUIColour[1]);
		lightColour[0].z = (imGUIColour[2]);
	}
}

//Blur Shader GUI
void App1::BlurGUI()
{
	const char* listbox_items[] = { "Cube", "Sphere", "Plane", "Triangle", "QuadMesh" };
	static int listbox_items_current = 1;
	ImGui::LabelText("Blur Post Processing Shader", "CURRENLY ON: ");

	//shape selection lsit
	if (ImGui::ListBox("Shape Selection", &listbox_items_current, listbox_items, 5))
	{
		switch (listbox_items_current)
		{
		case 0:
			//change shape
			selectedMesh = SelectedMesh::cube;
			break;
		case 1:
			//change shape
			selectedMesh = SelectedMesh::sphere;
			break;
		case 2:
			//change shape
			selectedMesh = SelectedMesh::plane;
			break;
		case 3:
			//change shape
			selectedMesh = SelectedMesh::triangle;
			break;
		case 4:
			//change shape
			selectedMesh = SelectedMesh::quad;
			break;
		case 5:
			break;
		}
	}

	//ambient color widget
	if (ImGui::ColorEdit3("Ambient color", imGUIAmbientColor)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		ambientColor.x = (imGUIAmbientColor[0]);
		ambientColor.y = (imGUIAmbientColor[1]);
		ambientColor.z = (imGUIAmbientColor[2]);
		ambientColor.w = 1.0f;
	}

	//weightings widget
	if (ImGui::InputFloat3("Weightings", imGUIVerticalWeightings))
	{
		vertWeightings.x = imGUIVerticalWeightings[0];
		vertWeightings.y = imGUIVerticalWeightings[1];
		vertWeightings.z = imGUIVerticalWeightings[2];
	}

}

//Light Shader GUI
void App1::LightGUI()
{
	const char* listbox_items[] = { "Cube", "Sphere", "Plane", "Triangle", "QuadMesh" };
	static int listbox_items_current = 1;
	ImGui::LabelText("Light Shader", "CURRENLY ON: ");

	//shape selection list
	if (ImGui::ListBox("Shape Selection", &listbox_items_current, listbox_items, 5))
	{
		switch (listbox_items_current)
		{
		case 0:
			//change shape
			selectedMesh = SelectedMesh::cube;
			break;
		case 1:
			//change shape
			selectedMesh = SelectedMesh::sphere;
			break;
		case 2:
			//change shape
			selectedMesh = SelectedMesh::plane;
			break;
		case 3:
			//change shape
			selectedMesh = SelectedMesh::triangle;
			break;
		case 4:
			//change shape
			selectedMesh = SelectedMesh::quad;
			break;
		case 5:
			break;
		}
	}

	//background color widget
	if (ImGui::ColorEdit3("Background color", imGUIBackgroundColour)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		backgroundColour.x = (imGUIBackgroundColour[0]);
		backgroundColour.y = (imGUIBackgroundColour[1]);
		backgroundColour.z = (imGUIBackgroundColour[2]);
	}

	//ambient color widget
	if (ImGui::ColorEdit3("Ambient color", imGUIAmbientColor)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		ambientColor.x = (imGUIAmbientColor[0]);
		ambientColor.y = (imGUIAmbientColor[1]);
		ambientColor.z = (imGUIAmbientColor[2]);
		ambientColor.w = 1.0f;
	}


	ImGui::CollapsingHeader("Light: 1");

	//light direction widget
	if (ImGui::InputFloat3("Light Direction 1", imGUILightDirection)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		lightDirection[0].x = (imGUILightDirection[0]);
		lightDirection[0].y = (imGUILightDirection[1]);
		lightDirection[0].z = (imGUILightDirection[2]);
	}

	//light color widget
	if (ImGui::InputFloat3("Light color 1", imGUIColour)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		lightColour[0].x = (imGUIColour[0]);
		lightColour[0].y = (imGUIColour[1]);
		lightColour[0].z = (imGUIColour[2]);
	}


	ImGui::CollapsingHeader("Light: 2");

	if (ImGui::InputFloat3("Light Direction 2", imGUILightDirection2)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		lightDirection[1].x = (imGUILightDirection2[0]);
		lightDirection[1].y = (imGUILightDirection2[1]);
		lightDirection[1].z = (imGUILightDirection2[2]);
	}

	if (ImGui::InputFloat3("Light color 2", imGUIColour2)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		lightColour[1].x = (imGUIColour2[0]);
		lightColour[1].y = (imGUIColour2[1]);
		lightColour[1].z = (imGUIColour2[2]);
	}



	ImGui::CollapsingHeader("Light: 3");

	if (ImGui::InputFloat3("Light Direction 3", imGUILightDirection3)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		lightDirection[2].x = (imGUILightDirection3[0]);
		lightDirection[2].y = (imGUILightDirection3[1]);
		lightDirection[2].z = (imGUILightDirection3[2]);
	}

	if (ImGui::InputFloat3("Light color 3", imGUIColour3)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		lightColour[2].x = (imGUIColour3[0]);
		lightColour[2].y = (imGUIColour3[1]);
		lightColour[2].z = (imGUIColour3[2]);
	}



	ImGui::CollapsingHeader("Light: 4");

	if (ImGui::InputFloat3("Light Direction 4", imGUILightDirection4)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		lightDirection[3].x = (imGUILightDirection4[0]);
		lightDirection[3].y = (imGUILightDirection4[1]);
		lightDirection[3].z = (imGUILightDirection4[2]);
	}

	if (ImGui::InputFloat3("Light color 4", imGUIColour4)) {
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		lightColour[3].x = (imGUIColour4[0]);
		lightColour[3].y = (imGUIColour4[1]);
		lightColour[3].z = (imGUIColour4[2]);
	}

}

//Vertex Manipulation GUI
void App1::VertexManipGUI()
{
	const char* listbox_items[] = { "Cube", "Sphere", "Plane", "Triangle", "QuadMesh" };
	static int listbox_items_current = 1;
	ImGui::LabelText("Vertex Manipulation Shader", "CURRENLY ON: ");


	if (ImGui::ListBox("Shape Selection", &listbox_items_current, listbox_items, 5))
	{
		switch (listbox_items_current)
		{
		case 0:
			selectedMesh = SelectedMesh::cube;
			break;
		case 1:
			selectedMesh = SelectedMesh::sphere;
			break;
		case 2:
			selectedMesh = SelectedMesh::plane;
			break;
		case 3:
			selectedMesh = SelectedMesh::triangle;
			break;
		case 4:
			selectedMesh = SelectedMesh::quad;
			break;
		case 5:
			break;
		}
	}

	ImGui::DragFloat("Speed: ", &speed, 0.1f);
	ImGui::DragFloat("Height: ", &height, 0.1f);
}

//tessellation GUI
void App1::TessellationGUI()
{
	ImGui::LabelText("Tessellation Shader", "CURRENLY ON: ");

	const char* listbox_items[] = { "Triangle", "Sphere"};
	static int listbox_items_current = 1;

	if (ImGui::ListBox("Shape Selection", &listbox_items_current, listbox_items, 2))
	{
		switch (listbox_items_current)
		{
		case 0:
			selectedTessMesh = SelectedTessMesh::tessTriangle;
			break;
		case 1:
			selectedTessMesh = SelectedTessMesh::tessSphere;
			break;
		}
	}
}

//Shadow shader GUI
void App1::ShadowGUI()
{
	const char* listbox_items[] = { "Sphere", "drone", "cube" };
	static int listbox_items_current = 1;
	ImGui::LabelText("Light Shader", "CURRENLY ON: ");


	if (ImGui::ListBox("Shape Selection", &listbox_items_current, listbox_items, 3))
	{
		switch (listbox_items_current)
		{
		case 0:
			selectedMesh = SelectedMesh::sphere;
			break;
		case 1:
			selectedMesh = SelectedMesh::drone;
			break;
		case 2:
			selectedMesh = SelectedMesh::cube;
			break;
		}
	}
	ImGui::CollapsingHeader("Light: 1");

	if (ImGui::InputFloat3("Light position 1", imGUIShadowLightPos))
	{
		// this code gets called if color value changes, so
		// the background color is upgraded automatically!
		XMFLOAT3 temp;

		temp.x = imGUIShadowLightPos[0];
		temp.y = imGUIShadowLightPos[1];
		temp.z = imGUIShadowLightPos[2];
		
		if (temp.x == shadowLightLookat.x && temp.y == shadowLightLookat.y && temp.z == shadowLightLookat.z)
		{		
		}
		else
		{
			shadowLightPos.x = imGUIShadowLightPos[0];
			shadowLightPos.y = imGUIShadowLightPos[1];
			shadowLightPos.z = imGUIShadowLightPos[2];
		}
	}
}


//=============Blur Cpp Methods===========
//render whole scene to texture
void App1::RenderToTexture()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	renderTexture->setRenderTarget(renderer->getDeviceContext());
	renderTexture->clearRenderTarget(renderer->getDeviceContext(), backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);

	camera->update();

	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	renderer->setZBuffer(false);
	chosenMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("Default"));
	textureShader->render(renderer->getDeviceContext(), chosenMesh->getIndexCount());
	renderer->setZBuffer(true);
	renderer->setBackBufferRenderTarget();
}

//down sample the rendered texture
void App1::DownSample()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;;


	downSampleTexture->setRenderTarget(renderer->getDeviceContext());
	downSampleTexture->clearRenderTarget(renderer->getDeviceContext(), backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);

	camera->update();

	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	renderer->setZBuffer(false);

	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, renderTexture->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);

	renderer->setBackBufferRenderTarget();
}

//apply horizontal blur
void App1::HorizontalBlur()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;;

	downSampleTargetWindowHorz->setRenderTarget(renderer->getDeviceContext());
	downSampleTargetWindowHorz->clearRenderTarget(renderer->getDeviceContext(), backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);

	camera->update();

	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	renderer->setZBuffer(false);
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, _screenWidth, downSampleTexture->getShaderResourceView(), horzWeightings);
	horizontalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);
	renderer->setBackBufferRenderTarget();
}

//apply vertical blur
void App1::VerticalBlur()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;;

	downSampleTargetWindowVert->setRenderTarget(renderer->getDeviceContext());
	downSampleTargetWindowVert->clearRenderTarget(renderer->getDeviceContext(), backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);

	camera->update();

	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	renderer->setZBuffer(false);
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, _screenHeight, downSampleTargetWindowHorz->getShaderResourceView(), vertWeightings);
	verticalBlurShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);
	renderer->setBackBufferRenderTarget();
}

//up sample texture
void App1::UpSample()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;;

	upSample->setRenderTarget(renderer->getDeviceContext());
	upSample->clearRenderTarget(renderer->getDeviceContext(), backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);

	camera->update();

	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	renderer->setZBuffer(false);
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, downSampleTargetWindowVert->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);
	renderer->setBackBufferRenderTarget();
}

//render scene normally
void App1::RenderScene()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;

	//// Clear the scene. (default blue colour)
	renderer->beginScene(backgroundColour.x, backgroundColour.y, backgroundColour.z, 1.0f);

	// Generate the view matrix based on the camera's position.
	camera->update();

	//// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	worldMatrix = renderer->getWorldMatrix();
	viewMatrix = camera->getViewMatrix();
	projectionMatrix = renderer->getProjectionMatrix();

	chosenMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture("Default"));
	textureShader->render(renderer->getDeviceContext(), chosenMesh->getIndexCount());

	renderer->setZBuffer(false);
	orthoMatrix = renderer->getOrthoMatrix();
	orthoViewMatrix = camera->getOrthoViewMatrix();

	orthoMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, upSample->getShaderResourceView());
	textureShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());

	renderer->setZBuffer(true);

	gui();
	renderer->endScene();
}



//void App1::VolumetricLighting()
//{
//	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix;;
//
//	camera->update();
//
//	worldMatrix = renderer->getWorldMatrix();
//	viewMatrix = camera->getViewMatrix();
//	projectionMatrix = renderer->getProjectionMatrix();
//
//	//renderer->setZBuffer(false);
//	orthoMatrix = renderer->getOrthoMatrix();
//	orthoViewMatrix = camera->getOrthoViewMatrix();
//
//	orthoMesh->sendData(renderer->getDeviceContext());
//	volumetricShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, orthoViewMatrix, orthoMatrix, renderTexture->getShaderResourceView(), m_light);
//	volumetricShader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
//
//	//renderer->setZBuffer(true);
//	renderer->setBackBufferRenderTarget();
//
//
//}