#include "Camera.h"
using namespace DirectX;

Camera::Camera(float aspect, float _fov, XMFLOAT3 position) : transform(position), fov(_fov)
{
	UpdateProjectionMatrix(aspect);
	UpdateViewMatrix();
}

Camera::~Camera()
{
	UpdateProjectionMatrix(16.f/9);
	UpdateViewMatrix();
}

XMFLOAT4X4 Camera::GetViewMatrix()
{
	return viewMatrix;
}

XMFLOAT4X4 Camera::GetProjectionMatrix()
{
	return projMatrix;
}

TransformEuler& Camera::GetTransform()
{
	return transform;
}

float Camera::GetFOV()
{
	return fov;
}


void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMMATRIX matrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, 0.1f, 30);
	XMStoreFloat4x4(&projMatrix, matrix);
}

void Camera::UpdateViewMatrix()
{
	XMVECTOR pos = XMVectorSet(transform.GetPosition().x, transform.GetPosition().y, transform.GetPosition().z, 0);
	XMVECTOR forward = XMVectorSet(transform.GetForward().x, transform.GetForward().y, transform.GetForward().z, 0);
	XMMATRIX matrix = XMMatrixLookToLH(pos, forward, XMVectorSet(0, 1, 0,0));
	XMStoreFloat4x4(&viewMatrix, matrix);
}

void Camera::Update(float dt)
{
	if (InputManager::KeyDown('W'))
	{
		transform.MoveRelative(0, 0, moveSpeed * dt);
		isDirty = true;
	}

	if (InputManager::KeyDown('S'))
	{
		transform.MoveRelative(0, 0, -moveSpeed * dt);
		isDirty = true;
	}

	if (InputManager::KeyDown('A'))
	{
		transform.MoveRelative(-moveSpeed * dt, 0, 0);
		isDirty = true;
	}

	if (InputManager::KeyDown('D'))
	{
		transform.MoveRelative(moveSpeed * dt, 0, 0);
		isDirty = true;
	}

	if (InputManager::KeyDown(VK_SPACE))
	{
		transform.MoveAbsolute(0, moveSpeed * dt, 0);
		isDirty = true;
	}

	if (InputManager::KeyDown('X'))
	{
		transform.MoveAbsolute(0, -moveSpeed * dt, 0);
		isDirty = true;
	}

	if (InputManager::MouseLeftDown()) {
		isDirty = true;

		transform.Rotate(0, InputManager::GetMouseXDelta() * mouseSensitivity, 0);
		// Clamp the pitch rotation
		transform.Rotate(InputManager::GetMouseYDelta() * mouseSensitivity, 0, 0);

		if (transform.GetPitchYawRoll().x > XM_PIDIV2 || transform.GetPitchYawRoll().x < -XM_PIDIV2)
			transform.Rotate(-InputManager::GetMouseYDelta() * mouseSensitivity, 0, 0);
	}

	if(isDirty)
		UpdateViewMatrix();
}