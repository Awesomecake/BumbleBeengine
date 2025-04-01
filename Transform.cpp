#include "Transform.h"

Transform::Transform() : position(0, 0, 0), scale(1, 1, 1)
{
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&worldInverseTransposeMatrix, DirectX::XMMatrixIdentity());
	XMStoreFloat4(&quaternion, DirectX::XMQuaternionIdentity());

	isDirty = false;
}

Transform::Transform(DirectX::XMFLOAT3 pos) : Transform()
{
	position = pos;
}

Transform::~Transform()
{

}

void Transform::SetPosition(float x, float y, float z)
{
	position = DirectX::XMFLOAT3(x, y, z);
	isDirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 newPosition)
{
	position = newPosition;
	isDirty = true;
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	XMStoreFloat4(&quaternion, DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));

	isDirty = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 newRotation)
{
	XMStoreFloat4(&quaternion, DirectX::XMQuaternionRotationRollPitchYaw(newRotation.x, newRotation.y, newRotation.z));
	isDirty = true;
}

void Transform::SetQuaternion(float x, float y, float z, float w)
{
	quaternion = DirectX::XMFLOAT4(x, y, z, w);
}

void Transform::SetScale(float x, float y, float z)
{
	scale = DirectX::XMFLOAT3(x, y, z);
	isDirty = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 newScale)
{
	scale = newScale;
	isDirty = true;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

DirectX::XMFLOAT4 Transform::GetRotation()
{
	return quaternion;
}


DirectX::XMFLOAT3 Transform::GetScale()
{
	return scale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (isDirty)
	{
		DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * DirectX::XMMatrixRotationQuaternion(quat) * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		XMStoreFloat4x4(&worldMatrix, world);
		XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
		isDirty = false;
	}

	return worldMatrix;
}

DirectX::XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	if (isDirty)
	{
		DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
		DirectX::XMMATRIX world = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * DirectX::XMMatrixRotationQuaternion(quat) * DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		XMStoreFloat4x4(&worldMatrix, world);
		XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));
		isDirty = false;
	}

	return worldInverseTransposeMatrix;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	DirectX::XMVECTOR rightVector = DirectX::XMVectorSet(1, 0, 0, 0);
	DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	rightVector = DirectX::XMVector3Rotate(rightVector, quat);

	DirectX::XMFLOAT3 newRightVec = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&newRightVec, rightVector);
	return newRightVec;
}

DirectX::XMFLOAT3 Transform::GetUp()
{
	DirectX::XMVECTOR upVector = DirectX::XMVectorSet(0, 1, 0, 0);
	DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	upVector = DirectX::XMVector3Rotate(upVector, quat);

	DirectX::XMFLOAT3 newUpVec = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&newUpVec, upVector);
	return newUpVec;
}

DirectX::XMFLOAT3 Transform::GetForward()
{
	DirectX::XMVECTOR forwardVector = DirectX::XMVectorSet(0, 0, 1, 0);
	DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	forwardVector = DirectX::XMVector3Rotate(forwardVector, quat);

	DirectX::XMFLOAT3 newForwardVec = DirectX::XMFLOAT3();
	DirectX::XMStoreFloat3(&newForwardVec, forwardVector);
	return newForwardVec;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;

	isDirty = true;
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	position.x += offset.x;
	position.y += offset.y;
	position.z += offset.z;

	isDirty = true;
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	XMStoreFloat4(&quaternion, DirectX::XMQuaternionMultiply(quat, DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll)));

	isDirty = true;
}

void Transform::Rotate(DirectX::XMFLOAT3 rotationOffset)
{
	DirectX::XMVECTOR quat = DirectX::XMVectorSet(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	XMStoreFloat4(&quaternion, DirectX::XMQuaternionMultiply(quat, DirectX::XMQuaternionRotationRollPitchYaw(rotationOffset.x, rotationOffset.y, rotationOffset.z)));

	isDirty = true;
}

void Transform::Scale(float x, float y, float z)
{
	scale.x *= x;
	scale.y *= y;
	scale.z *= z;

	isDirty = true;
}

void Transform::Scale(DirectX::XMFLOAT3 scaleAmount)
{
	scale.x *= scaleAmount.x;
	scale.y *= scaleAmount.y;
	scale.z *= scaleAmount.z;

	isDirty = true;
}