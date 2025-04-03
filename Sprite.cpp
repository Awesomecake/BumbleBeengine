#include "Sprite.h"

Sprite::Sprite(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material)
{
	mesh = refMesh;
	material = _material;
}


Sprite::~Sprite()
{

}

TransformEuler& Sprite::GetTransform()
{
	return transform;
}

std::shared_ptr<Mesh> Sprite::GetMesh()
{
	return mesh;
}

std::shared_ptr<Material> Sprite::GetMaterial()
{
	return material;
}

void Sprite::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera)
{
	DirectX::XMFLOAT2 mousePos = DirectX::XMFLOAT2((float)InputManager::GetMouseX(), (float)InputManager::GetMouseY());
	material->PrepareMaterial();

	//Set Pixel Shader and Load Data
	material->pixelShader->SetFloat4("surfaceColor", material->surfaceColor);
	material->pixelShader->SetFloat2("mousePos", mousePos);
	material->pixelShader->SetFloat("roughness", material->roughness);
	material->pixelShader->SetFloat3("cameraPos", camera->GetTransform().GetPosition());

	material->pixelShader->CopyAllBufferData();

	//Set Vertex Shader and Load Data
	material->vertexShader->SetMatrix4x4("world", transform.GetWorldMatrix());
	material->vertexShader->SetMatrix4x4("view", camera->GetViewMatrix());
	material->vertexShader->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	material->vertexShader->SetMatrix4x4("worldInvTranspose", transform.GetWorldInverseTransposeMatrix());

	material->vertexShader->CopyAllBufferData();

	mesh->Draw(context);
}

void Sprite::SetMaterial(std::shared_ptr<Material> newMat)
{
	material = newMat;
}