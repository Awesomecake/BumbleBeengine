#include "GameEntity.h"

GameEntity::GameEntity(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material)
{
	mesh = refMesh;
	material = _material;
	usingPhysicsBody = false;
}

GameEntity::GameEntity(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material, JPH::BodyID physBody) : mesh(refMesh), material(_material), physicsBody(physBody)
{
	usingPhysicsBody = true;
}

void GameEntity::UpdateTransformFromPhysicsBody(PhysicsManager* physicsManager)
{
	RVec3 position = physicsManager->body_interface->GetCenterOfMassPosition(physicsBody);
	Vec3 rotation = physicsManager->body_interface->GetRotation(physicsBody).GetEulerAngles();

	transform.SetPosition(position.GetX(), position.GetY(), position.GetZ());
	transform.SetRotation(rotation.GetX(), rotation.GetY(), rotation.GetZ());
}

GameEntity::~GameEntity()
{

}

Transform& GameEntity::GetTransform()
{
	return transform;
}

std::shared_ptr<Mesh> GameEntity::GetMesh()
{
	return mesh;
}

std::shared_ptr<Material> GameEntity::GetMaterial()
{
	return material;
}

JPH::BodyID GameEntity::GetPhysicsBody() { return physicsBody; }
bool GameEntity::GetIsUsingPhysics() { return usingPhysicsBody; }

void GameEntity::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera)
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

void GameEntity::SetMaterial(std::shared_ptr<Material> newMat)
{
	material = newMat;
}