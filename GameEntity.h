#pragma once
#include "Transform.h"
#include "Mesh.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"
#include "Lights.h"
#include "PhysicsManager.h"

#include <memory>

class GameEntity
{
private:
	Transform transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;

	bool usingPhysicsBody;
	JPH::BodyID physicsBody;

public:

	GameEntity(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material);
	GameEntity(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material, JPH::BodyID physBody);
	GameEntity() {}
	~GameEntity();
	std::shared_ptr<Mesh> GetMesh();
	Transform& GetTransform();
	std::shared_ptr<Material> GetMaterial();
	JPH::BodyID GetPhysicsBody();
	bool GetIsUsingPhysics();
	void UpdateTransformFromPhysicsBody(PhysicsManager* physicsManager);


	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera);
	void SetMaterial(std::shared_ptr<Material> newMat);
};