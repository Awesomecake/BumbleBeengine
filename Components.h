#pragma once
#include "Mesh.h"
#include "Material.h"
#include <memory>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>

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

	//Data Structure is designed to be sent to the shader
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

struct AudioComponent
{

};

