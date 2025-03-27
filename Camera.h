#pragma once
#include "InputManager.h"
#include "TransformEuler.h"
#include <DirectXMath.h>

class Camera
{
private:
	TransformEuler transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;

	float fov = 45.f;

public:
	Camera(float aspect, float _fov, DirectX::XMFLOAT3 position);
	~Camera();

	DirectX::XMFLOAT4X4 GetViewMatrix();
	DirectX::XMFLOAT4X4 GetProjectionMatrix();
	TransformEuler& GetTransform();
	float GetFOV();

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt);

	bool isDirty = false;
	float mouseSensitivity = 0.0050f;
	float moveSpeed = 1.f;

};