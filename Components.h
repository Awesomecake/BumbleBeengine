#pragma once
#include "Mesh.h"
#include "Material.h"
#include <memory>

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
	TransformComponent() : position(0, 0, 0), scale(1, 1, 1)
	{
		DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
		DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
		XMStoreFloat4(&quaternion, DirectX::XMQuaternionIdentity());

		isDirty = false;
	}

	TransformComponent(DirectX::XMFLOAT3 pos) : TransformComponent()
	{
		position = pos;
	}

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 quaternion;

	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;

	bool isDirty;
};

struct PhysicsComponent
{

};

struct CameraComponent
{

};

struct LightComponent
{

};

//struct ScriptComponent
//{
//
//};

struct AudioComponent
{

};

