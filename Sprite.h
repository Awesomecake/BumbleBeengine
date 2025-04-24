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
	int isFlipped;

	DrawRect(float rectW, float rectH, float imgW, float imgH)
		: xOffset(0), yOffset(0), rectWidth(rectW), rectHeight(rectH), imgWidth(imgW), imgHeight(imgH), isFlipped(0) {}
	// Constructor to initialize the struct with default values
	DrawRect() : xOffset(0), yOffset(0), rectWidth(0), rectHeight(0), imgWidth(0), imgHeight(0), isFlipped(0) {}


};

struct AnimationData {

	std::shared_ptr<std::vector<std::tuple<int, int>>> animation;
	//std::shared_ptr<std::tuple<int, int>> currentFrame;
	float frameRate;

	AnimationData(std::shared_ptr<std::vector<std::tuple<int, int>>> anim, float rate)
		: animation(anim), frameRate(rate) {}
	
};

struct AnimationDataDictionary {
	std::shared_ptr<std::vector<AnimationData>> dictionary;
	std::shared_ptr<std::vector<int>> animFrameCounts;
	int currentAnimationIndex;
	int currentFrameIndex;
	
	AnimationDataDictionary(std::shared_ptr<std::vector<AnimationData>> dict)
		: dictionary(dict), currentAnimationIndex(0), currentFrameIndex(0) {}
	//AnimationDataDictionary() : currentAnimationIndex(0), currentFrameIndex(0) {}
};

static std::shared_ptr<AnimationDataDictionary> CreateAnimDataDictionary(std::shared_ptr<DrawRect> drawRect, std::shared_ptr<std::vector<int>> animFrameCounts);

class Sprite
{
private:
	TransformEuler transform;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	float timeElapsed;

public:

	int scale;
	Sprite(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material, std::shared_ptr<DrawRect> _drawRect, std::shared_ptr<std::vector<int>> animFrameCounts);
	//Sprite() {}
	~Sprite();
	std::shared_ptr<Mesh> GetMesh();
	TransformEuler& GetTransform();
	std::shared_ptr<Material> GetMaterial();
	std::shared_ptr<AnimationDataDictionary> animationDataDictionary;
	std::shared_ptr<DrawRect> drawRect;

	void Update(float deltaTime);
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera);
	void SetMaterial(std::shared_ptr<Material> newMat);
	void SetDrawRect(float column, float row, float rectW, float rectH, float imgW, float imgH);
	//void CreateAnimDataDictionary();
	
};


