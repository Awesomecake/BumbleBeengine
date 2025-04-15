#pragma once
#include "TransformEuler.h"
#include "Mesh.h"
#include "Camera.h"
#include "SimpleShader.h"
#include "Material.h"

#include <memory>

struct DrawRect
{
	float xOffset;
	float yOffset;
	float rectWidth;
	float rectHeight;
	float imgWidth;
	float imgHeight;

	DrawRect(float xOff, float yOff, float rectW, float rectH, float imgW, float imgH)
		: xOffset(xOff), yOffset(yOff), rectWidth(rectW), rectHeight(rectH), imgWidth(imgW), imgHeight(imgH) {}
	// Constructor to initialize the struct with default values
	DrawRect() : xOffset(0), yOffset(0), rectWidth(0), rectHeight(0), imgWidth(0), imgHeight(0) {}
};

struct AnimationData {

	std::shared_ptr<std::vector<std::tuple<int, int>>> animation;
	std::shared_ptr<std::tuple<int, int>> currentFrame;
	float frameRate;
};

struct AnimationDataDictionary {
	std::shared_ptr<std::vector<AnimationData>> dictionary;
	int currentAnimationIndex;
	int currentFrameIndex;
};

class Sprite
{
private:
	TransformEuler transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	std::shared_ptr<DrawRect> drawRect;

public:

	Sprite(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material);
	Sprite() {}
	~Sprite();
	std::shared_ptr<Mesh> GetMesh();
	TransformEuler& GetTransform();
	std::shared_ptr<Material> GetMaterial();
	AnimationDataDictionary animationDataDictionary;


	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera);
	void SetMaterial(std::shared_ptr<Material> newMat);
	void SetDrawRect(float column, float row, float rectW, float rectH, float imgW, float imgH);
	
};


