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

