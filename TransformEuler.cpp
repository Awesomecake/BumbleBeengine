#include "TransformEuler.h"

TransformEuler::TransformEuler() : position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1)
{
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());

	isDirty = false;
}

TransformEuler::TransformEuler(DirectX::XMFLOAT3 pos) : TransformEuler()
{
	position = pos;
}

TransformEuler::~TransformEuler()
{

}

void TransformEuler::SetPosition(float x, float y, float z)
{
	position = DirectX::XMFLOAT3(x, y, z);
	isDirty = true;
}

void TransformEuler::SetPosition(DirectX::XMFLOAT3 newPosition)
{
	position = newPosition;
	isDirty = true;
}

void TransformEuler::SetRotation(float pitch, float yaw, float roll)
{
	rotation = DirectX::XMFLOAT3(pitch, yaw, roll);
	isDirty = true;
}

void TransformEuler::SetRotation(DirectX::XMFLOAT3 newRotation) // XMFLOAT4 for quaternion
{
	rotation = newRotation;
	isDirty = true;
}

void TransformEuler::SetScale(float x, float y, float z)
{
	scale = DirectX::XMFLOAT3(x, y, z);
	isDirty = true;
}

void TransformEuler::SetScale(DirectX::XMFLOAT3 newScale)
{
	scale = newScale;
	isDirty = true;
}

DirectX::XMFLOAT3 TransformEuler::GetPosition()
{
	return position;
}

DirectX::XMFLOAT3 TransformEuler::GetPitchYawRoll() // XMFLOAT4 GetRotation() for quaternion
{
	return rotation;
}

DirectX::XMFLOAT3 TransformEuler::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 TransformEuler::GetWorldMatrix()
{
	if (isDirty)
	{
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		XMStoreFloat4x4(&worldMatrix, world);
		XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
		isDirty = false;
	}

	return worldMatrix;
}

DirectX::XMFLOAT4X4 TransformEuler::GetWorldInverseTransposeMatrix()
{
	if (isDirty)
	{
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z) * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		XMStoreFloat4x4(&worldMatrix, world);
		XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
		isDirty = false;
	}

	return worldInverseTransposeMatrix;
}

DirectX::XMFLOAT3 TransformEuler::GetRight()
{
	DirectX::XMVECTOR rightVector = DirectX::XMVectorSet(1, 0, 0, 0);
	rightVector = DirectX::XMVector3Rotate(rightVector, DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));

	DirectX::XMFLOAT3 newRightVec = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&newRightVec, rightVector);
	return newRightVec;
}

DirectX::XMFLOAT3 TransformEuler::GetUp()
{
	DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0, 1, 0, 0);
	upVector = DirectX::XMVector3Rotate(upVector, DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));

	DirectX::XMFLOAT3 newUpVec = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&newUpVec, upVector);
	return newUpVec;
}

DirectX::XMFLOAT3 TransformEuler::GetForward()
{
	DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0, 0, 1, 0);
	forwardVector = DirectX::XMVector3Rotate(forwardVector, DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z));

	DirectX::XMFLOAT3 newForwardVec = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&newForwardVec, forwardVector);
	return newForwardVec;
}

void TransformEuler::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;

	isDirty = true;
}

void TransformEuler::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	position.x += offset.x;
	position.y += offset.y;
	position.z += offset.z;

	isDirty = true;
}

void TransformEuler::MoveRelative(float x, float y, float z)
{
	DirectX::XMVECTOR moveOffset = DirectX::XMVectorSet(x, y, z, 0.0f);
	DirectX::XMVECTOR quat = DirectX::XMQuaternionRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	moveOffset = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), DirectX::XMVector3Rotate(moveOffset, quat));

	DirectX::XMStoreFloat3(&position, moveOffset);

	isDirty = true;
}

void TransformEuler::Rotate(float pitch, float yaw, float roll)
{
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;

	isDirty = true;
}

void TransformEuler::Rotate(DirectX::XMFLOAT3 rotationOffset)
{
	rotation.x += rotationOffset.x;
	rotation.y += rotationOffset.y;
	rotation.z += rotationOffset.z;

	isDirty = true;
}

void TransformEuler::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;

	isDirty = true;
}

void TransformEuler::Scale(DirectX::XMFLOAT3 scaleAmount)
{
	scale.x *= scaleAmount.x;
	scale.y *= scaleAmount.y;
	scale.z *= scaleAmount.z;

	isDirty = true;
}