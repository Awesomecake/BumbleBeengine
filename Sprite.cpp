#include "Sprite.h"

#define Sprite_Assets L"../../Assets/SpriteSheets/"

Sprite::Sprite(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material, std::shared_ptr<DrawRect> _drawRect, std::shared_ptr<std::vector<int>> animFrameCounts)
{
	mesh = refMesh;
	material = _material;
	scale = 1;
	//hard coded values for testing rn
	drawRect = _drawRect;
	animationDataDictionary = CreateAnimDataDictionary(_drawRect, animFrameCounts);
	timeElapsed = 0.0f;
	
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

void Sprite::Update(float deltaTime) 
{
	timeElapsed += deltaTime;
	if (timeElapsed >= 1/(animationDataDictionary->dictionary->at(animationDataDictionary->currentAnimationIndex).frameRate))
	{
		timeElapsed = 0.0f;
		animationDataDictionary->currentFrameIndex++;
		if (animationDataDictionary->currentFrameIndex >= animationDataDictionary->dictionary->at(animationDataDictionary->currentAnimationIndex).animation->size())
		{
			animationDataDictionary->currentFrameIndex = 0;
		}
		drawRect->xOffset = std::get<1>(animationDataDictionary->dictionary->at(animationDataDictionary->currentAnimationIndex).animation->at(animationDataDictionary->currentFrameIndex));
		//drawRect->yOffset = std::get<0>(animationDataDictionary->dictionary->at(animationDataDictionary->currentAnimationIndex).animation->at(animationDataDictionary->currentFrameIndex));
	}
	drawRect->yOffset =animationDataDictionary->currentAnimationIndex * drawRect->rectHeight;
	if(animationDataDictionary->currentAnimationIndex != 0)
	{
		//wow
	}
}

void Sprite::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context, std::shared_ptr<Camera> camera)
{
	material->PrepareMaterial();

	//Set Pixel Shader and Load Data
	material->pixelShader->SetFloat4("surfaceColor", material->surfaceColor);
	material->pixelShader->SetFloat("xOffset", drawRect->xOffset);
	material->pixelShader->SetFloat("yOffset", drawRect->yOffset);
	material->pixelShader->SetFloat("rectWidth", drawRect->rectWidth);
	material->pixelShader->SetFloat("rectHeight", drawRect->rectHeight);
	material->pixelShader->SetFloat("imgWidth", drawRect->imgWidth);
	material->pixelShader->SetFloat("imgHeight", drawRect->imgHeight);
	material->pixelShader->SetInt("isFlipped", drawRect->isFlipped);

	transform.SetScale(scale, 1.0f, drawRect->imgHeight / drawRect->imgWidth * scale);

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

void Sprite::SetDrawRect(float column, float row, float rectW, float rectH, float imgW, float imgH)
{
	drawRect->xOffset = column * rectW;
	drawRect->yOffset = row * rectH;
	drawRect->rectWidth = rectW;
	drawRect->rectHeight = rectH;
	drawRect->imgWidth = imgW;
	drawRect->imgHeight = imgH;
}

static std::shared_ptr<AnimationDataDictionary> CreateAnimDataDictionary(std::shared_ptr<DrawRect> drawRect, std::shared_ptr<std::vector<int>> animFrameCounts)
{
	// Create a shared pointer to a vector of AnimationData
	std::shared_ptr<std::vector<AnimationData>> animDataVector = std::make_shared<std::vector<AnimationData>>();
	// Create a shared pointer to the AnimationDataDictionary
	std::shared_ptr<AnimationDataDictionary> animDataDict = std::make_shared<AnimationDataDictionary>(animDataVector);

	//loop through each animation in the sprite sheet (assuming 1 unique animation per row)
	for(int i = 0; i < drawRect->imgHeight/drawRect->rectHeight; i++)
	{
		std::shared_ptr<std::vector<std::tuple<int, int>>> anim = std::make_shared<std::vector<std::tuple<int, int>>>();
		//loop through each frame in the animation 
		for(int j = 0; j < drawRect->imgWidth/drawRect->rectWidth; j++)
		{
			if(j >= animFrameCounts->at(i))
				break;
			anim->push_back(std::make_tuple(i* drawRect->rectHeight, j* drawRect->rectWidth));

		}
		AnimationData animData = AnimationData(anim, 12);
		animDataDict->dictionary->push_back(animData);
	}
	animDataDict->currentAnimationIndex = 0;
	animDataDict->currentFrameIndex = 0;
	return animDataDict;
}
