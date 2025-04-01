#pragma once
#include "Components.h"
#include <DirectXMath.h>

static class Systems
{
public:
	static DirectX::XMFLOAT4X4	GetWorldMatrix(TransformComponent transform_comp);
	static DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix(TransformComponent transform_comp);

};