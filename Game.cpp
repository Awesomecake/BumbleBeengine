#include "Game.h"
#include "Vertex.h"
#include "InputManager.h"
#include "PathHelpers.h"
#include "Mesh.h"
#include <string>
#include "WICTextureLoader.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

// Needed for a helper function to load pre-compiled shader files
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

#define PBR_Assets L"../../Assets/PBR/"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// Direct3D itself, and our window, are not ready at this point!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,			// The application's handle
		L"DirectX Game",	// Text for the window's title bar (as a wide-character string)
		1280,				// Width of the window's client area
		720,				// Height of the window's client area
		false,				// Sync the framerate to the monitor refresh? (lock framerate)
		true)				// Show extra stats (fps) in title bar?
{
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Delete all objects manually created within this class
//  - Release() all Direct3D objects created within this class
// --------------------------------------------------------
Game::~Game()
{
	// Call delete or delete[] on any objects or arrays you've
	// created using new or new[] within this class
	// - Note: this is unnecessary if using smart pointers

	// Call Release() on any Direct3D objects made within this class
	// - Note: this is unnecessary for D3D objects stored in ComPtrs

	// ImGui clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	physicsManager->DeInitPhysics();
}

// Function to be called by the delegate
void CollisionCallback(AudioManager* audioManager) {
	audioManager->playSound("Sounds/vine-boom.wav");
}

// --------------------------------------------------------
// Called once per program, after Direct3D and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;		// Can make this a "Graphics Setting"
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());

	// Sampler state for post processing
	D3D11_SAMPLER_DESC ppSampDesc = {};
	ppSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ppSampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&ppSampDesc, ppSampler.GetAddressOf());

	postProcess1 = PostProcess(device,windowWidth,windowHeight, ppSampler, ppPS1);
	postProcess1.pixelShaderFloatData.insert({ "sharpenAmount", &sharpenAmount });

	postProcess2 = PostProcess(device, windowWidth, windowHeight, ppSampler, ppPS2);
	postProcess2.pixelShaderFloatData.insert({ "blurRadius", &blurAmount });

	postProcess3 = PostProcess(device, windowWidth, windowHeight, ppSampler, ppPS3);
	postProcess3.pixelShaderFloatData.insert({ "pixelLevel", &pixelIntensity });

	postProcess4 = PostProcess(device, windowWidth, windowHeight, ppSampler, ppPS4);
	postProcess4.pixelShaderFloatData.insert({ "mouseX", &mouseX });
	postProcess4.pixelShaderFloatData.insert({ "mouseY", &mouseY });

	shadowMap = ShadowMap(device, shadowMapVertexShader, windowWidth, windowHeight);

	CreateMaterial(PBR_Assets "floor_albedo.png", PBR_Assets "floor_normals.png", PBR_Assets "floor_roughness.png", PBR_Assets "floor_metal.png");
	CreateMaterial(PBR_Assets "bronze_albedo.png", PBR_Assets "bronze_normals.png", PBR_Assets "bronze_roughness.png", PBR_Assets "bronze_metal.png");
	CreateMaterial(PBR_Assets "cobblestone_albedo.png", PBR_Assets "cobblestone_normals.png", PBR_Assets "cobblestone_roughness.png", PBR_Assets "cobblestone_metalness.png");
	CreateMaterial(PBR_Assets "scratched_albedo.png", PBR_Assets "scratched_normals.png", PBR_Assets "scratched_roughness.png", PBR_Assets "scratched_metal.png");

	CreateGeometry();

	sky = std::make_shared<Sky>(cube, samplerState, device, context, FixPath(L"../../Assets/Skies/Planet/").c_str());

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Initialize ImGui itself & platform/renderer backends
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(device.Get(), context.Get());
	// Pick a style (uncomment one of these 3)
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();
	//ImGui::StyleColorsClassic();

	cameras = std::vector<std::shared_ptr<Camera>>();
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 45.f, XMFLOAT3(0.5, 0, -5)));
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 90.f, XMFLOAT3(-0.5, 0, -5)));
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 45.f, XMFLOAT3(0, 0.5, -5)));
	cameras.push_back(std::make_shared<Camera>((float)this->windowWidth / this->windowHeight, 90.f, XMFLOAT3(0, -0.5, -5)));

	physicsManager = new PhysicsManager();
	BodyID sphere1 = physicsManager->CreatePhysicsSphereBody(RVec3(0.0_r, 20.0_r, 0.0_r), 1);
	physicsManager->AddBodyVelocity(sphere1, Vec3(0.0f, -5.0f, 0.0f));
	BodyID sphere2 = physicsManager->CreatePhysicsSphereBody(RVec3(0.1_r, 0.0_r, 0.1_r), 1);

	physicsManager->contact_listener.collisionDelegate = CollisionCallback;

	entt::entity testEntity = registry.create();
	registry.emplace<TransformComponent>(testEntity, XMFLOAT3(0, 20, 0));
	registry.emplace<MeshComponent>(testEntity, sphere);
	registry.emplace<MaterialComponent>(testEntity, materials[0]);
	registry.emplace<PhysicsComponent>(testEntity, sphere1);

	entt::entity testEntity2 = registry.create();
	registry.emplace<TransformComponent>(testEntity2, XMFLOAT3(0,0,0));
	registry.emplace<MeshComponent>(testEntity2, sphere);
	registry.emplace<MaterialComponent>(testEntity2, materials[0]);
	registry.emplace<PhysicsComponent>(testEntity2, sphere2);

#pragma region Constructing Lights
	lights = std::vector<Light>();

	Light light3 = Light();
	light3.Direction = XMFLOAT3(0, -1, -1);
	light3.Color = XMFLOAT3(1, 1, 1);

	lights.push_back(light3);

	Light light = Light();
	light.Type = LIGHT_TYPE_POINT;
	light.Color = XMFLOAT3(1, 0, 0);
	light.Position = XMFLOAT3(0, 0, 0);
	light.Range = 10;
	light.Intensity = 2;

	lights.push_back(light);

	Light light2 = Light();
	light2.Type = LIGHT_TYPE_POINT;
	light2.Color = XMFLOAT3(0, 0, 1);
	light2.Position = XMFLOAT3(0, 0, 5);
	light2.Range = 10;
	light2.Intensity = 2;

	lights.push_back(light2);

	Light light4 = light3;
	light4.Direction = XMFLOAT3(-1, 0, 0);
	light4.Color = XMFLOAT3(1, 1, 0);

	lights.push_back(light4);

	Light light5 = light3;
	light5.Direction = XMFLOAT3(0, 0, -1);
	light5.Color = XMFLOAT3(0, 1, 1);

	lights.push_back(light5);
#pragma endregion

	shadowMap.MakeProjection(light3.Direction);

	audioManager = std::make_shared<AudioManager>();
	InitializeInputActions();

}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	pixelShader = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PixelShader.cso").c_str());
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"VertexShader.cso").c_str());
	
	shadowMapVertexShader = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"ShadowMapVertexShader.cso").c_str());

	ppPS1 = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PostProcessSharpenPS.cso").c_str());
	ppPS2 = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PostProcessBlurPS.cso").c_str());
	ppPS3 = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PostProcessPixelizePS.cso").c_str());
	ppPS4 = std::make_shared<SimplePixelShader>(device, context, FixPath(L"PostProcessChromaticAberrationPS.cso").c_str());

	ppVS = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"FullScreenTriangle.cso").c_str());
}

void Game::InitializeInputActions()
{
	InputActionManager::CreateAction(L"Look");
	InputActionManager::AssignBindingToAction(L"Look", InputActionManager::InputBindings::XControllerLeftStick);

	InputActionManager::GetAction(L"Look").OnTrigger.push_back([&](InputActionManager::InputData data)
		{
			if (data.inputType == InputActionManager::InputType::Value && data.controllerIndex == CONTROLLER_1)
			{
				std::optional<XMFLOAT2> vector = data.value.GetValue<XMFLOAT2>();
				float x = vector.value().x;
				float y = vector.value().y;

				if (vector.value().x < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && vector.value().x > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) { x = 0; }
				if (vector.value().y < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE && vector.value().y > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) { y = 0; }

				if (x != 0 || y != 0)
				{
					cameras[selectedCamera]->isDirty = true;

					cameras[selectedCamera]->GetTransform().Rotate(0, vector.value().x / 32768.0f * cameras[selectedCamera]->mouseSensitivity, 0);
					// Clamp the pitch rotation
					cameras[selectedCamera]->GetTransform().Rotate(-vector.value().y / 32768.0f * cameras[selectedCamera]->mouseSensitivity, 0, 0);

					if (cameras[selectedCamera]->GetTransform().GetPitchYawRoll().x > XM_PIDIV2 || cameras[selectedCamera]->GetTransform().GetPitchYawRoll().x < -XM_PIDIV2)
						cameras[selectedCamera]->GetTransform().Rotate(vector.value().y / 32768.0f * cameras[selectedCamera]->mouseSensitivity, 0, 0);
				}
				
			}
		});
}

void Game::CreateMaterial(std::wstring albedoFile, std::wstring normalFile, std::wstring roughnessFile, std::wstring metalnessFile)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> albedoSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalsSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> roughnessSRV;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalnessSRV;

	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(albedoFile).c_str(), nullptr, albedoSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(normalFile).c_str(), nullptr, normalsSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(roughnessFile).c_str(), nullptr, roughnessSRV.GetAddressOf());
	CreateWICTextureFromFile(device.Get(), context.Get(), FixPath(metalnessFile).c_str(), nullptr, metalnessSRV.GetAddressOf());

	std::shared_ptr<Material> mat = std::make_shared<Material>(XMFLOAT4(1, 1, 1, 1), pixelShader, vertexShader);
	mat->textureSRVs.insert({ "Albedo", albedoSRV });
	mat->textureSRVs.insert({ "NormalMap", normalsSRV });
	mat->textureSRVs.insert({ "RoughnessMap", roughnessSRV });
	mat->textureSRVs.insert({ "MetalnessMap", metalnessSRV });
	mat->samplers.insert({ "BasicSampler",samplerState });
	mat->PrepareMaterial();

	materials.push_back(mat);
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateGeometry()
{
	cube = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cube.igme540obj").c_str(), device);
	cylinder = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/cylinder.igme540obj").c_str(), device);
	helix = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/helix.igme540obj").c_str(), device);
	sphere = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/sphere.igme540obj").c_str(), device);
	torus = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/torus.igme540obj").c_str(), device);
	quad = std::make_shared<Mesh>(FixPath(L"../../Assets/Models/quad.igme540obj").c_str(), device);

	entt::entity entity1 = registry.create();
	registry.emplace<TransformComponent>(entity1, XMFLOAT3(-9, -3, 0));
	registry.emplace<MeshComponent>(entity1, cube);
	registry.emplace<MaterialComponent>(entity1, materials[0]);

	entt::entity entity2 = registry.create();
	registry.emplace<TransformComponent>(entity2, XMFLOAT3(-6, -3, 0));
	registry.emplace<MeshComponent>(entity2, cylinder);
	registry.emplace<MaterialComponent>(entity2, materials[0]);

	entt::entity entity3 = registry.create();
	registry.emplace<TransformComponent>(entity3, XMFLOAT3(-3, -3, 0));
	registry.emplace<MeshComponent>(entity3, helix);
	registry.emplace<MaterialComponent>(entity3, materials[0]);

	entt::entity entity4 = registry.create();
	registry.emplace<TransformComponent>(entity4, XMFLOAT3(0, -3, 0));
	registry.emplace<MeshComponent>(entity4, sphere);
	registry.emplace<MaterialComponent>(entity4, materials[1]);

	entt::entity entity5 = registry.create();
	registry.emplace<TransformComponent>(entity5, XMFLOAT3(3, -3, 0));
	registry.emplace<MeshComponent>(entity5, torus);
	registry.emplace<MaterialComponent>(entity5, materials[1]);

	entt::entity entity6 = registry.create();
	registry.emplace<TransformComponent>(entity6, XMFLOAT3(6, -3, 0));
	registry.emplace<MeshComponent>(entity6, quad);
	registry.emplace<MaterialComponent>(entity6, materials[1]);

	entt::entity entity7 = registry.create();
	registry.emplace<TransformComponent>(entity7, XMFLOAT3(0, -7, 0), XMFLOAT3(20, 1, 20));
	registry.emplace<MeshComponent>(entity7, cube);
	registry.emplace<MaterialComponent>(entity7, materials[2]);
}


// --------------------------------------------------------
// Handle resizing to match the new window size.
//  - DXCore needs to resize the back buffer
//  - Eventually, we'll want to update our 3D camera
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	postProcess1.Resize(device, windowWidth, windowHeight);
	postProcess2.Resize(device, windowWidth, windowHeight);
	postProcess3.Resize(device, windowWidth, windowHeight);
	postProcess4.Resize(device, windowWidth, windowHeight);

	shadowMap.Resize(windowWidth, windowHeight);

	for (size_t i = 0; i < cameras.size(); i++)
	{
		cameras[i].get()->UpdateProjectionMatrix((float)this->windowWidth / this->windowHeight);
	}
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (InputManager::KeyDown(VK_ESCAPE))
		Quit();

	if (InputManager::KeyPress(VK_END))
	{
		audioManager->playSound("Sounds/vine-boom.wav");
	}

	cameras[selectedCamera].get()->Update(deltaTime);
	ImGuiUpdate(deltaTime, totalTime);
	BuildUI(deltaTime, totalTime);

	mouseX = (InputManager::GetMouseX() / (float) windowWidth);
	mouseY = (InputManager::GetMouseY() / (float)windowHeight);

	//gameEntities[3].GetTransform().SetPosition(3*cos(totalTime), -3, 3*sin(totalTime));
	//gameEntities[2].GetTransform().Rotate(deltaTime, 0, deltaTime);

	
#pragma region Physics System

	if (InputManager::KeyPress(VK_DELETE))
	{
		int matLocation = rand() % materials.size();

		XMFLOAT3 camPos = cameras[selectedCamera]->GetTransform().GetPosition();
		XMFLOAT3 camForward = cameras[selectedCamera]->GetTransform().GetForward();

		BodyID id = physicsManager->CreatePhysicsSphereBody(Vec3(camPos.x, camPos.y, camPos.z), 0.5);
		physicsManager->AddBodyVelocity(id, Vec3(camForward.x * 10, camForward.y * 10, camForward.z * 10));

		entt::entity shotEntity = registry.create();
		registry.emplace<TransformComponent>(shotEntity, camPos, XMFLOAT3(0.5, 0.5, 0.5));
		registry.emplace<MeshComponent>(shotEntity, sphere);
		registry.emplace<MaterialComponent>(shotEntity, materials[matLocation]);
		registry.emplace<PhysicsComponent>(shotEntity, id);
	}

	if (InputManager::KeyPress(VK_INSERT))
	{
		//XMFLOAT3 pos = cameras[0]->GetTransform().GetPosition();
		//XMFLOAT3 forward = cameras[0]->GetTransform().GetForward();

		//AllHitCollisionCollector<RayCastBodyCollector> collector = physicsManager->JoltRayCast(Vec3(pos.x, pos.y, pos.z), Vec3Arg(forward.x, forward.y, forward.z), 100);

		//bool hasHit = collector.HadHit();

		//if (hasHit)
		//{
		//	for (auto& hitBody : collector.mHits)
		//	{
		//		if (bodyObjects.contains(hitBody.mBodyID))
		//		{
		//			bodyObjects[hitBody.mBodyID].SetMaterial(materials[1]);
		//		}
		//	}
		//}
	}
	
	timeSincePhysicsStep += deltaTime;

	while (timeSincePhysicsStep >= cDeltaTime && runPhysics)
	{
		physicsManager->JoltPhysicsFrame();
		timeSincePhysicsStep -= cDeltaTime;

		// Next step
		++step;

		auto mycompMesh = registry.view<TransformComponent, PhysicsComponent>();
		for (auto [entity, transform_comp, physics_comp] : mycompMesh.each())
		{
			Systems::UpdateTransformFromPhysicsBody(physicsManager, physics_comp, transform_comp);
		}
	}

#pragma endregion

#pragma region Audio handling

	audioManager->update_audio(deltaTime);

#pragma endregion

}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Frame START
	// - These things should happen ONCE PER FRAME
	// - At the beginning of Game::Draw() before drawing *anything*
	{
		// Clear the back buffer (erases what's on the screen)
		float bgColor[4] = { ambientColor.x, ambientColor.y, ambientColor.z, 1};
		context->ClearRenderTargetView(backBufferRTV.Get(), bgColor);

		postProcess1.ClearRTV(context, bgColor);
		postProcess2.ClearRTV(context, bgColor);
		postProcess3.ClearRTV(context, bgColor);
		postProcess4.ClearRTV(context, bgColor);

		// Clear the depth buffer (resets per-pixel occlusion information)
		context->ClearDepthStencilView(depthBufferDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	}

	//shadowMap.DrawShadowMap(context,registry,backBufferRTV, depthBufferDSV);

	context->OMSetRenderTargets(1, postProcess1.ppRTV.GetAddressOf(), depthBufferDSV.Get()); //Setup First Post Processing Target
	
	RenderScene();

	ppVS->SetShader();
	//postProcess1.RenderPostProcess(context, postProcess2.ppRTV, depthBufferDSV);
	//postProcess2.RenderPostProcess(context, postProcess3.ppRTV, depthBufferDSV);
	//postProcess3.RenderPostProcess(context, postProcess4.ppRTV, depthBufferDSV);
	postProcess1.RenderPostProcess(context, backBufferRTV, 0);

	ImGui::Render(); // Turns this frame’s UI into renderable triangles
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // Draws it to the screen

	// Frame END
	// - These should happen exactly ONCE PER FRAME
	// - At the very end of the frame (after drawing *everything*)
	{
		ID3D11ShaderResourceView* nullSRVs[128] = {};
		context->PSSetShaderResources(0, 128, nullSRVs);

		// Present the back buffer to the user
		//  - Puts the results of what we've drawn onto the window
		//  - Without this, the user never sees anything
		bool vsyncNecessary = vsync || !deviceSupportsTearing || isFullscreen;
		swapChain->Present(vsyncNecessary ? 1 : 0, vsyncNecessary ? 0 : DXGI_PRESENT_ALLOW_TEARING);

		// Must re-bind buffers after presenting, as they become unbound
		context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthBufferDSV.Get());
	}
}

void Game::RenderScene()
{
	auto mycompMesh = registry.view<MeshComponent, MaterialComponent, TransformComponent>();
	for (auto [entity, mesh_comp, material_comp, transform_comp] : mycompMesh.each())
	{
		material_comp.material->pixelShader->SetShaderResourceView("ShadowMap", shadowMap.shadowSRV.Get());

		material_comp.material->pixelShader->SetShaderResourceView("ShadowMap", shadowMap.shadowSRV.Get());
		material_comp.material->pixelShader->SetSamplerState("ShadowSampler", shadowMap.shadowSampler);
		material_comp.material->pixelShader->SetData("lights", &lights[0], sizeof(Light) * (int)lights.size());
		
		material_comp.material->vertexShader->SetMatrix4x4("lightView", shadowMap.shadowViewMatrix);
		material_comp.material->vertexShader->SetMatrix4x4("lightProjection", shadowMap.shadowProjectionMatrix);

		{
			DirectX::XMFLOAT2 mousePos = DirectX::XMFLOAT2((float)InputManager::GetMouseX(), (float)InputManager::GetMouseY());
			material_comp.material->PrepareMaterial();

			//Set Pixel Shader and Load Data
			material_comp.material->pixelShader->SetFloat4("surfaceColor", material_comp.material->surfaceColor);
			material_comp.material->pixelShader->SetFloat2("mousePos", mousePos);
			material_comp.material->pixelShader->SetFloat("roughness", material_comp.material->roughness);
			material_comp.material->pixelShader->SetFloat3("cameraPos", cameras[selectedCamera]->GetTransform().GetPosition());

			material_comp.material->pixelShader->CopyAllBufferData();

			//Set Vertex Shader and Load Data
			material_comp.material->vertexShader->SetMatrix4x4("world", Systems::CalcWorldMatrix(transform_comp));
			material_comp.material->vertexShader->SetMatrix4x4("view", cameras[selectedCamera]->GetViewMatrix());
			material_comp.material->vertexShader->SetMatrix4x4("projection", cameras[selectedCamera]->GetProjectionMatrix());
			material_comp.material->vertexShader->SetMatrix4x4("worldInvTranspose", Systems::CalcWorldInverseTransposeMatrix(transform_comp));

			material_comp.material->vertexShader->CopyAllBufferData();

			mesh_comp.mesh->Draw(context);
		}
	}

	sky->ambient = ambientColor;
	sky->Draw(context, cameras[selectedCamera]);
}

#pragma region ImGui
void Game::ImGuiUpdate(float deltaTime, float totalTime)
{
	// Feed fresh data to ImGui
	ImGuiIO& io = ImGui::GetIO();
	io.DeltaTime = deltaTime;
	io.DisplaySize.x = (float)this->windowWidth;
	io.DisplaySize.y = (float)this->windowHeight;
	// Reset the frame
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	//Determine the new input capture state which is what the user is currently doing
	InputManager::SetKeyboardCapture(io.WantCaptureKeyboard); // make sure no ! flag on this..
	InputManager::SetMouseCapture(io.WantCaptureMouse);
	// Show the demo window

	if(showImGuiDemoWindow)
		ImGui::ShowDemoWindow();
}

void Game::BuildUI(float deltaTime, float totalTime)
{
	ImVec4 detailsColor = ImVec4(0.7f, 0.7f, 1, 1);

	ImGui::Begin("Inspector"); // Everything after is part of the window

	if (ImGui::TreeNode("App Details"))
	{
		ImGui::TextColored(detailsColor, " - Framerate: %f fps", ImGui::GetIO().Framerate);
		ImGui::TextColored(detailsColor, " - Window Resolution: %dx%d", windowWidth, windowHeight);
		ImGui::ColorEdit3("Ambient Color", &ambientColor.x);

		// Create a button and test for a click
		if (!showImGuiDemoWindow)
		{
			if (ImGui::Button("Show ImGui Demo Window"))
				showImGuiDemoWindow = true;
		}
		else
		{
			if (ImGui::Button("Hide ImGui Demo Window"))
				showImGuiDemoWindow = false;
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Meshes"))
	{
		ImGui::TextColored(detailsColor, " - Mesh 0: %u triangle(s)", cube.get()->GetIndexCount()/3);
		ImGui::TextColored(detailsColor, " - Mesh 1: %u triangle(s)", cylinder.get()->GetIndexCount()/3);
		ImGui::TextColored(detailsColor, " - Mesh 2: %u triangle(s)", helix.get()->GetIndexCount()/3);
		ImGui::TextColored(detailsColor, " - Mesh 3: %u triangle(s)", sphere.get()->GetIndexCount() / 3);
		ImGui::TextColored(detailsColor, " - Mesh 4: %u triangle(s)", torus.get()->GetIndexCount() / 3);
		ImGui::TextColored(detailsColor, " - Mesh 5: %u triangle(s)", quad.get()->GetIndexCount() / 3);

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Scene Entities"))
	{
		auto mycompMesh = registry.view<MeshComponent, MaterialComponent, TransformComponent>(); // mesh	

		int i = 0;
		for (auto [entity, mesh_comp, material_comp, transform_comp] : mycompMesh.each())
		{
			i++;

			std::string string = "Entity " + std::to_string(i);
			if (ImGui::TreeNode(string.data()))
			{
				XMFLOAT3 position = transform_comp.GetPosition();
				ImGui::DragFloat3("Position", &position.x, 0.005f, -5.0f, 5.0f, "%.3f");
				transform_comp.SetPosition(position);

				XMFLOAT3 scale = transform_comp.GetScale();
				ImGui::DragFloat3("Scale", &scale.x, 0.005f, -5.0f, 5.0f, "%.3f");
				transform_comp.SetScale(scale);

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Current Camera"))
	{
		std::string string = "Camera #" + std::to_string(selectedCamera);
		ImGui::SeparatorText(string.c_str());

		ImGui::TextColored(detailsColor, "Camera Details:");
		{
			XMFLOAT3 position = cameras[selectedCamera].get()->GetTransform().GetPosition();
			ImGui::TextColored(detailsColor, "	Position: %0.2f, %0.2f, %0.2f", position.x, position.y, position.z);

			XMFLOAT3 rotation = cameras[selectedCamera].get()->GetTransform().GetPitchYawRoll();
			ImGui::TextColored(detailsColor, "	Rotation (Radians): %0.2f, %0.2f, %0.2f", rotation.x, rotation.y, rotation.z);

			float fov = cameras[selectedCamera].get()->GetFOV();
			ImGui::TextColored(detailsColor, "	FOV: %0.2f", fov);
		}

		ImGui::Spacing();
		ImGui::Spacing();

		if (ImGui::Button("Change To Previous Camera"))
		{
			selectedCamera = (selectedCamera - 1) % cameras.size();
		}
		ImGui::SameLine();

		if (ImGui::Button("Change To Next Camera"))
		{
			selectedCamera = (selectedCamera + 1) % cameras.size();
		}


		ImGui::TreePop();
	}

	if (ImGui::TreeNode("Lights"))
	{
		for (int i = 0; i < lights.size(); i++)
		{
			std::string string = "Light #" + std::to_string(i+1) + " - ";

			switch (lights[i].Type)
			{
			case LIGHT_TYPE_DIRECTIONAL:
				string += "Directional";
				break;
			case LIGHT_TYPE_POINT:
				string += "Point";
				break;
			case LIGHT_TYPE_SPOT:
				string += "Spot";
				break;
			}

			if (ImGui::TreeNode(string.data()))
			{
				XMFLOAT3 color = lights[i].Color;
				ImGui::DragFloat3("Color", &color.x, 0.005f, 0.f, 1.0f, "%.01f");
				lights[i].Color = color;

				float intensity = lights[i].Intensity;
				ImGui::DragFloat("Intensity", &intensity, 0.005f, 0.0f, 50.0f, "%.3f");
				lights[i].Intensity = intensity;

				ImGui::TreePop();
			}
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Textures"))
	{
		if (ImGui::Button("Cycle Texture"))
		{
			auto mycompMesh = registry.view<MeshComponent, MaterialComponent, TransformComponent>(); // mesh	

			int i = 0;
			for (auto [entity, mesh_comp, material_comp, transform_comp] : mycompMesh.each())
			{
				ImGuiMaterialIndex++;
				material_comp.material = materials[ImGuiMaterialIndex % materials.size()];
			}
		}

		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Post Processing"))
	{
		ImGui::DragFloat("Sharpen Intensity", &sharpenAmount, 0.1f, 0, 10, "%.01f");
		ImGui::DragFloat("Blur Intensity", &blurAmount, 0.1f, 0, 10, "%.01f");
		ImGui::DragFloat("Pixel Intensity", &pixelIntensity, 0.1f, 0, 10, "%.01f");

		ImGui::TreePop();
	}

	if (ImGui::Button("Run Physics"))
	{
		runPhysics = true;
		timeSincePhysicsStep = 0.f;
	}

	ImGui::End(); // Ends the current window
}
#pragma endregion