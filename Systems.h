#pragma once
#include "Components.h"
#include <DirectXMath.h>
#include "EnTT/include/entt/entt.hpp"
#include "PhysicsManager.h"

static class Systems
{
public:
	static DirectX::XMFLOAT4X4	CalcWorldMatrix(TransformComponent& transform_comp);
	static DirectX::XMFLOAT4X4 CalcWorldInverseTransposeMatrix(TransformComponent& transform_comp);
	DirectX::XMFLOAT3 GetRight(TransformComponent& transform_comp);
	DirectX::XMFLOAT3 GetUp(TransformComponent& transform_comp);
	DirectX::XMFLOAT3 GetForward(TransformComponent& transform_comp);

	static void UpdateTransformFromPhysicsBody(PhysicsManager* physicsManager, PhysicsComponent& physics_comp, TransformComponent& transform_comp);

	static void DrawSkyBox(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera, SkyBoxComponent skybox_comp);

	static Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubeMap(
		Microsoft::WRL::ComPtr<ID3D11Device> device,
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		const wchar_t* right,
		const wchar_t* left,
		const wchar_t* up,
		const wchar_t* down,
		const wchar_t* front,
		const wchar_t* back);
};