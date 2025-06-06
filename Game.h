#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h> // Used for ComPtr - a smart pointer for COM objects

#include "Mesh.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"
#include "ShadowMap.h"
#include "PostProcess.h"
#include "AudioManager.h"
#include "Sprite.h"

#include "UIManager.h"
#include "Canvases.h"

#include "PhysicsManager.h"
#include "XInputManager.h"

#include "EnTT/include/entt/entt.hpp"
#include "Components.h"
#include "Systems.h"

#include "Emitter.h"

#include <memory>
#include <vector>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void ImGuiUpdate(float deltaTime, float totalTime);
	void BuildUI(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void RenderScene();

	//ImGui test value
	bool showImGuiDemoWindow = false;
	bool randomizeColorOffset = false;
	int ImGuiMaterialIndex = 0;
	DirectX::XMFLOAT3 ambientColor = { 0.5f,0.5f,0.5f };

	//Camera
	std::vector<std::shared_ptr<Camera>> cameras;
	int selectedCamera = 0;

	float deltaTime = 0.f;

	//Meshes
	std::shared_ptr<Mesh> cube;
	std::shared_ptr<Mesh> cylinder;
	std::shared_ptr<Mesh> helix;
	std::shared_ptr<Mesh> sphere;
	std::shared_ptr<Mesh> torus;
	std::shared_ptr<Mesh> quad;

	//Sprites
	std::shared_ptr<Sprite> testSprite;
	std::shared_ptr<DrawRect> drawRect;

	//Materials
	std::vector<std::shared_ptr<Material>> materials;

	//Shadow Map
	ShadowMap shadowMap;

	//Post Processing
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	std::shared_ptr<SimpleVertexShader> ppVS;

	PostProcess postProcess1;
	PostProcess postProcess2;
	PostProcess postProcess3;
	PostProcess postProcess4;

	float blurAmount = 0;
	float pixelIntensity = 0;
	float sharpenAmount = 0;

	float mouseX;
	float mouseY;

	// Audio
	std::shared_ptr<AudioManager> audioManager;

	// UI
	UIManager* uiManager;

private:

	// Initialization helper methods - feel free to customize, combine, remove, etc.
	void LoadShaders(); 
	void CreateGeometry();
	void CreateParticleResources();
	void InitializeInputActions();
	std::shared_ptr<Material> CreateMaterial(std::wstring albedoFile, std::wstring normalFile, std::wstring roughnessFile, std::wstring metalnessFile);
	std::shared_ptr<Material> CreateSpriteMaterial(std::wstring albedoFile);
	std::shared_ptr<Material> CreateParticleMaterial(std::wstring albedoFile);

	std::shared_ptr<SimplePixelShader> pixelShader;
	std::shared_ptr<SimplePixelShader> ppPS1;
	std::shared_ptr<SimplePixelShader> ppPS2;
	std::shared_ptr<SimplePixelShader> ppPS3;
	std::shared_ptr<SimplePixelShader> ppPS4;

	std::shared_ptr<SimplePixelShader> spritePixelShader;

	std::shared_ptr<SimpleVertexShader> vertexShader;
	std::shared_ptr<SimpleVertexShader> shadowMapVertexShader;

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> spriteSamplerState;

	//--------------Particles------------------

	//particle shader data
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> particleDepthState;
	Microsoft::WRL::ComPtr<ID3D11BlendState> particleBlendState;

	//loader particle shaders
	std::shared_ptr<SimpleVertexShader> particleVertexShader;
	std::shared_ptr<SimplePixelShader> particlePixelShader;

	std::vector<std::shared_ptr<Emitter>> emitters;

	//--------------JoltPhysics----------------
	PhysicsManager* physicsManager;

	uint step = 0;

	// We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
	const float cDeltaTime = 1.0f / 60.0f;
	float timeSincePhysicsStep = 0.f;
	bool runPhysics = true;

	entt::registry registry;

	entt::entity entity2D;
};

