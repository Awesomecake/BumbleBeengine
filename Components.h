#pragma once
#include "Mesh.h"
#include "Material.h"
#include <memory>
#include <x3daudio.h>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

#pragma region tempHeaders
#include "Camera.h"
#include <WICTextureLoader.h>
#include "PathHelpers.h"

#pragma endregion


struct MeshComponent
{
	std::shared_ptr<Mesh> mesh;
};

struct MaterialComponent
{
	std::shared_ptr<Material> material;
};

struct SpriteComponent
{
	//std::shared_ptr<Material> material;
};

struct TransformComponent
{
public:
	TransformComponent() : position(0, 0, 0), scale(1, 1, 1)
	{
		DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
		XMStoreFloat4(&quaternion, DirectX::XMQuaternionIdentity());

		isDirty = true;
	}

	TransformComponent(DirectX::XMFLOAT3 pos) : TransformComponent()
	{
		position = pos;
	}

	TransformComponent(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 _scale) : TransformComponent()
	{
		position = pos;
		scale = _scale;
	}

	const DirectX::XMFLOAT3& GetPosition() const {return position;}
	const void SetPosition(DirectX::XMFLOAT3 pos) { position = pos; isDirty = true; }

	const DirectX::XMFLOAT3 GetScale() const { return scale; }
	const void SetScale(DirectX::XMFLOAT3 newScale) { scale = newScale; isDirty = true; }

	const DirectX::XMFLOAT4 GetQuaternion() const {return quaternion; }
	const void SetQuaternion(float x, float y, float z, float w) { quaternion = DirectX::XMFLOAT4(x, y, z, w); isDirty = true; }
	const void SetQuaternionEuler(DirectX::XMFLOAT3 newRotation)
	{
		XMStoreFloat4(&quaternion, DirectX::XMQuaternionRotationRollPitchYaw(newRotation.x, newRotation.y, newRotation.z));
		isDirty = true;
	}

	const bool IsDirty() const { return isDirty; }
	const void SetIsDirty(bool dirty) { isDirty = dirty; }

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 quaternion;

	bool isDirty;
};

struct PhysicsComponent
{
	JPH::BodyID bodyID;
};

struct CameraComponent
{

};

struct SkyBoxComponent
{
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampleState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> stencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapTexture;

	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<SimplePixelShader> ps;
	std::shared_ptr<SimpleVertexShader> vs;

public:
	SkyBoxComponent(std::shared_ptr<Mesh> _mesh, Microsoft::WRL::ComPtr<ID3D11SamplerState> _sampleState, 
		Microsoft::WRL::ComPtr<ID3D11Device> device, Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, bool _isUsed)
	{
		sampleState = _sampleState;
		mesh = _mesh;

		D3D11_RASTERIZER_DESC rasterizerDescription = {};
		rasterizerDescription.FillMode = D3D11_FILL_SOLID;
		rasterizerDescription.CullMode = D3D11_CULL_FRONT;
		device->CreateRasterizerState(&rasterizerDescription, &rasterizerState);

		D3D11_DEPTH_STENCIL_DESC stencilDescription = {};
		stencilDescription.DepthEnable = true;
		stencilDescription.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		device->CreateDepthStencilState(&stencilDescription, &stencilState);

		ps = std::make_shared<SimplePixelShader>(device, context, FixPath(L"SkyPixelShader.cso").c_str());
		vs = std::make_shared<SimpleVertexShader>(device, context, FixPath(L"SkyVertexShader.cso").c_str());

		isUsed = _isUsed;
	}

	const std::shared_ptr<Mesh> GetMesh() const { return mesh; }
	const std::shared_ptr<SimplePixelShader> GetPixelShader() const { return ps; }
	const std::shared_ptr<SimpleVertexShader> GetVertexShader() const { return vs; }

	const Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSampleState() const { return sampleState; }
	const Microsoft::WRL::ComPtr<ID3D11DepthStencilState> GetStencilState() const { return stencilState; }
	const Microsoft::WRL::ComPtr<ID3D11RasterizerState> GetRasterizerState() const { return rasterizerState; }
	const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetCubeMapTexture() const { return cubeMapTexture; }
	const void SetCubeMapTexture(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> _cubeMapTexture) { cubeMapTexture = _cubeMapTexture; }

	DirectX::XMFLOAT3 ambient;
	bool isUsed;
};

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

struct LightComponent
{
	LightComponent() : Type(LIGHT_TYPE_DIRECTIONAL), Direction(1, 0, 0), Range(10), Position(0, 0, 0), Intensity(1), Color(1, 1, 1), SpotFalloff(0) { }
	//Type, Direction, Range, Position, Intensity, Color, SpotFallOff
	LightComponent(int _type, DirectX::XMFLOAT3 _direction, float _range, DirectX::XMFLOAT3 _position, float _intensity, DirectX::XMFLOAT3 _color, float _spotFallOff, bool _isShadowMap)
	{ 
		Type = _type;
		Direction = _direction;
		Range = _range;
		Position = _position;
		Intensity = _intensity;
		Color = _color;
		SpotFalloff = _spotFallOff;
		isShadowMap = _isShadowMap;
	}

	//Data Structure is designed to be sent to the pixel shader
	//Do Not Change
	int Type;
	DirectX::XMFLOAT3 Direction;
	float Range;
	DirectX::XMFLOAT3 Position;
	float Intensity;
	DirectX::XMFLOAT3 Color;
	float SpotFalloff;
	bool isShadowMap;
	DirectX::XMFLOAT2 Padding;
};

//struct ScriptComponent
//{
//
//};

struct AudioListenerComponent
{
public:
	void UpdateListener(X3DAUDIO_VECTOR front, X3DAUDIO_VECTOR up, X3DAUDIO_VECTOR pos,
		X3DAUDIO_VECTOR vel)
	{
		listener->OrientFront = transform->GetForward();
		listener->OrientTop = transform->GetUp();
		listener->Position = transform->GetPosition();
		//listener->Velocity = vel;
	}

	AudioListenerComponent(TransformEuler* transformEuler)
	{
		transform = transformEuler;
	}

	X3DAUDIO_LISTENER* GetListener()
	{
		return listener.get();
	}

private:
	std::shared_ptr<X3DAUDIO_LISTENER> listener;
	TransformEuler* transform;
};