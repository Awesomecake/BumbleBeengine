#include "Systems.h"

#pragma region TransformLogic
DirectX::XMFLOAT4X4	Systems::CalcWorldMatrix(TransformComponent& transform_comp)
{
	if (transform_comp.IsDirty())
	{
		//Get Values
		DirectX::XMFLOAT3 pos = transform_comp.GetPosition();
		DirectX::XMFLOAT3 scale = transform_comp.GetScale();
		DirectX::XMFLOAT4 quaternion = transform_comp.GetQuaternion();

		//Calculate Matrices
		DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) *
			DirectX::XMMatrixRotationQuaternion(quat) * 
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

		XMStoreFloat4x4(&transform_comp.worldMatrix, world);
		XMStoreFloat4x4(&transform_comp.worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
		transform_comp.SetIsDirty(false);
	}

	return transform_comp.worldMatrix;
}

DirectX::XMFLOAT4X4 Systems::CalcWorldInverseTransposeMatrix(TransformComponent& transform_comp)
{
	if (transform_comp.IsDirty())
	{
		//Get Values
		DirectX::XMFLOAT3 pos = transform_comp.GetPosition();
		DirectX::XMFLOAT3 scale = transform_comp.GetScale();
		DirectX::XMFLOAT4 quaternion = transform_comp.GetQuaternion();

		//Calculate Matrices
		DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * 
			DirectX::XMMatrixRotationQuaternion(quat) * 
			DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		
		XMStoreFloat4x4(&transform_comp.worldMatrix, world);
		XMStoreFloat4x4(&transform_comp.worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
		transform_comp.SetIsDirty(false);
	}

	return transform_comp.worldInverseTransposeMatrix;
}
#pragma endregion

#pragma region PhysicsLogic
void Systems::UpdateTransformFromPhysicsBody(PhysicsManager* physicsManager, PhysicsComponent& physics_comp, TransformComponent& transform_comp)
{
	RVec3 position = physicsManager->body_interface->GetCenterOfMassPosition(physics_comp.bodyID);
	JPH::Quat rotation = physicsManager->body_interface->GetRotation(physics_comp.bodyID);

	transform_comp.SetPosition(DirectX::XMFLOAT3(position.GetX(), position.GetY(), position.GetZ()));
	transform_comp.SetQuaternion(rotation.GetX(), rotation.GetY(), rotation.GetZ(), rotation.GetW());
}
#pragma endregion