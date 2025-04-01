#pragma once
#include "Components.h"
#include <DirectXMath.h>
#include "EnTT/include/entt/entt.hpp"

class Systems
{
public:
	static DirectX::XMFLOAT4X4	CalcWorldMatrix(TransformComponent& transform_comp);
	static DirectX::XMFLOAT4X4 CalcWorldInverseTransposeMatrix(TransformComponent& transform_comp);
};