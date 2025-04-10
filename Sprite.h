#pragma once
#include "TransformEuler.h"
#include "Mesh.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"

#include <memory>

class Sprite
{
private:
	TransformEuler transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;


public:

	Sprite(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material);
	Sprite() {}
	~Sprite();
	std::shared_ptr<Mesh> GetMesh();
	TransformEuler& GetTransform();
	std::shared_ptr<Material> GetMaterial();

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera);
	void SetMaterial(std::shared_ptr<Material> newMat);
};