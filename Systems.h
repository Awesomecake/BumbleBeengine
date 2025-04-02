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

	static void UpdateTransformFromPhysicsBody(PhysicsManager* physicsManager, PhysicsComponent& physics_comp, TransformComponent& transform_comp);
};