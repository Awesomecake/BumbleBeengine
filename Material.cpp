#include "Material.h"

Material::Material(DirectX::XMFLOAT4 _colorTint, std::shared_ptr<SimplePixelShader> _ps, std::shared_ptr<SimpleVertexShader> _vs)
	: surfaceColor(_colorTint), pixelShader(_ps), vertexShader(_vs) { }

void Material::PrepareMaterial()
{
	pixelShader->SetShader();
	vertexShader->SetShader();

	for (auto& t : textureSRVs) { pixelShader->SetShaderResourceView(t.first.c_str(), t.second); }
	for (auto& s : samplers) { pixelShader->SetSamplerState(s.first.c_str(), s.second); }
}

//returns the pixel shader
std::shared_ptr<SimplePixelShader> Material::GetPixelShader()
{
	return pixelShader;
}

//returns the vertex shader
std::shared_ptr<SimpleVertexShader> Material::GetVertexShader()
{
	return vertexShader;
}

Material::~Material()
{

}