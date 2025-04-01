#include "Systems.h"

DirectX::XMFLOAT4X4	Systems::GetWorldMatrix(TransformComponent transform_comp)
{
	if (transform_comp.isDirty)
	{
		DirectX::XMVECTOR quat = DirectX::XMVectorSet(transform_comp.quaternion.x, transform_comp.quaternion.y, transform_comp.quaternion.z, transform_comp.quaternion.w);
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(transform_comp.scale.x, transform_comp.scale.y, transform_comp.scale.z) *
			DirectX::XMMatrixRotationQuaternion(quat) * 
			DirectX::XMMatrixTranslation(transform_comp.position.x, transform_comp.position.y, transform_comp.position.z);

		XMStoreFloat4x4(&transform_comp.worldMatrix, world);
		XMStoreFloat4x4(&transform_comp.worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
		transform_comp.isDirty = false;
	}

	return transform_comp.worldMatrix;
}

DirectX::XMFLOAT4X4 Systems::GetWorldInverseTransposeMatrix(TransformComponent transform_comp)
{
	if (transform_comp.isDirty)
	{
		DirectX::XMVECTOR quat = DirectX::XMVectorSet(transform_comp.quaternion.x, transform_comp.quaternion.y, transform_comp.quaternion.z, transform_comp.quaternion.w);
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(transform_comp.scale.x, transform_comp.scale.y, transform_comp.scale.z) * 
			DirectX::XMMatrixRotationQuaternion(quat) * 
			DirectX::XMMatrixTranslation(transform_comp.position.x, transform_comp.position.y, transform_comp.position.z);
		
		XMStoreFloat4x4(&transform_comp.worldMatrix, world);
		XMStoreFloat4x4(&transform_comp.worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
		transform_comp.isDirty = false;
	}

	return transform_comp.worldInverseTransposeMatrix;
}
