#include "Sprite.h"

Sprite::Sprite(std::shared_ptr<Mesh> refMesh, std::shared_ptr<Material> _material)
{
	mesh = refMesh;
	material = _material;

	//hard coded values for testing rn
	drawRect = std::make_shared<DrawRect>(0, 0, 128, 80, 1280, 800);
	
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
	material->PrepareMaterial();

	//Set Pixel Shader and Load Data
	material->pixelShader->SetFloat4("surfaceColor", material->surfaceColor);
	material->pixelShader->SetFloat("xOffset", drawRect->xOffset);
	material->pixelShader->SetFloat("yOffset", drawRect->yOffset);
	material->pixelShader->SetFloat("rectWidth", drawRect->rectWidth);
	material->pixelShader->SetFloat("rectHeight", drawRect->rectHeight);
	material->pixelShader->SetFloat("imgWidth", drawRect->imgWidth);
	material->pixelShader->SetFloat("imgHeight", drawRect->imgHeight);

	transform.SetScale(1.0f, 1.0f, drawRect->imgHeight / drawRect->imgWidth);

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